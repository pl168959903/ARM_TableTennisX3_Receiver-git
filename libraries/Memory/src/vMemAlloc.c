
#include "vMemAlloc.h"

//預分配空間
static uint8_t memArr[ VM_MEM_SIZE ];

static size_t FreeSize = 0;

//鏈表結構
typedef struct stBlock {
    struct stBlock* nextBlock;
    size_t          size;
} stBlock_T;

//鏈表大小;對齊記憶體
static const size_t xBlockStructSize = ( sizeof( stBlock_T ) + ( size_t )( VM_ALIGNMENT_SIZE - 1 ) ) & ~( ( size_t )VM_ALIGNMENT_MASK );

//鏈表起點&終點
stBlock_T pxStart, *pxEnd = NULL;

/**
 * @brief  初始化
 * @note
 * @retval None
 */
static void vMemInit( void ) {
    stBlock_T* newBlock;
    uint8_t*   pAlignedAddr;
    size_t     xTotalSize = VM_MEM_SIZE;
    size_t     uAddr      = ( size_t )memArr;

    //對齊記憶體
    if ( ( uAddr & VM_ALIGNMENT_MASK ) != 0 ) {
        uAddr += ( size_t )( VM_ALIGNMENT_SIZE - 1 );
        uAddr &= ~( ( size_t )( VM_ALIGNMENT_MASK ) );
        xTotalSize -= ( uAddr - ( size_t )memArr );
    }
    FreeSize = xTotalSize;
    // 設定pxStart
    pAlignedAddr      = ( uint8_t* )uAddr;
    pxStart.nextBlock = ( void* )pAlignedAddr;
    pxStart.size      = ( size_t )0;

    //設定pxEnd
    xTotalSize -= xBlockStructSize;
    uAddr            = ( size_t )pAlignedAddr + xTotalSize;
    pxEnd            = ( void* )uAddr;
    pxEnd->nextBlock = NULL;
    pxEnd->size      = 0;

    //設定第一個block
    newBlock            = ( void* )pxStart.nextBlock;
    newBlock->nextBlock = pxEnd;
    newBlock->size      = ( size_t )( newBlock->nextBlock ) - ( ( size_t )newBlock + xBlockStructSize );
}
/**
 * @brief  分配空間
 * @note
 * @param  allocSize: 分配大小
 * @retval 分配空間指標
 */
void* vMemAlloc( size_t WantSize ) {
    stBlock_T* pVisitBlock         = NULL;
    stBlock_T* pPreviousVisitBlock = NULL;
    stBlock_T* pAllocBlock         = NULL;
    stBlock_T* pPreviousAllocBlock = NULL;
    stBlock_T* pNewBlock           = NULL;
    void*      pReturn             = NULL;
    size_t     allocSize           = WantSize;
    //檢查是否需要初始化
    if ( pxEnd == NULL ) vMemInit();

    //對齊記憶體;總申請記憶體大小
    if ( ( allocSize & VM_ALIGNMENT_MASK ) != 0 ) {
        allocSize += ( size_t )( VM_ALIGNMENT_SIZE - 1 );
        allocSize &= ~( ( size_t )( VM_ALIGNMENT_MASK ) );
    }

    // 走訪每一個空鏈表，並檢查能容納申請記憶體的最小鏈以及前一個連接的鏈
    pPreviousVisitBlock = &pxStart;
    pVisitBlock         = pPreviousVisitBlock->nextBlock;
    while ( pVisitBlock != pxEnd ) {
        //找到剛好放得下的鏈
        if ( pVisitBlock->size == allocSize ) {
            FreeSize -= ( allocSize + xBlockStructSize );
            pReturn                        = ( void* )( ( size_t )pVisitBlock + xBlockStructSize );
            pPreviousVisitBlock->nextBlock = pVisitBlock->nextBlock;
            return pReturn;
        }
        //找出能容納包含新鏈的空間
        if ( pVisitBlock->size >= ( allocSize + xBlockStructSize ) ) {
            if ( pAllocBlock == NULL || pVisitBlock->size < pAllocBlock->size ) {
                pAllocBlock         = pVisitBlock;
                pPreviousAllocBlock = pPreviousVisitBlock;
            }
        }
        pPreviousVisitBlock = pVisitBlock;
        pVisitBlock         = pVisitBlock->nextBlock;
    }

    //如果找到合適的分配鏈，分配空間
    if ( pAllocBlock != NULL ) {
        FreeSize -= ( allocSize + xBlockStructSize );
        pReturn                        = ( void* )( ( size_t )pAllocBlock + xBlockStructSize );
        pNewBlock                      = ( void* )( ( ( size_t )pAllocBlock + xBlockStructSize ) + allocSize );
        pNewBlock->nextBlock           = pAllocBlock->nextBlock;
        pNewBlock->size                = pAllocBlock->size - ( xBlockStructSize + allocSize );
        pAllocBlock->size              = allocSize;
        pPreviousAllocBlock->nextBlock = pNewBlock;
    }
    return pReturn;
}

/**
 * @brief  釋放空間
 * @note
 * @param  pv: 空間指標
 * @retval None
 */
void vMemFree( void* pv ) {
    stBlock_T* pVisitBlock         = NULL;
    stBlock_T* pPreviousVisitBlock = NULL;
    stBlock_T* pFreeBlock          = ( void* )( ( size_t )pv - xBlockStructSize );

    //走訪鏈表
    if ( pv != NULL ) {
        pPreviousVisitBlock = &pxStart;
        pVisitBlock         = pPreviousVisitBlock->nextBlock;
        while ( pVisitBlock != NULL ) {
            //尋找大於pv位址的最小位址block
            if ( ( size_t )pVisitBlock > ( size_t )pFreeBlock ) {
                FreeSize += ( xBlockStructSize + pFreeBlock->size );
                //檢查釋放空間尾部是否剛好連接鏈表，如果連接，將他們合成。
                if ( ( ( size_t )pFreeBlock + xBlockStructSize + pFreeBlock->size ) == ( size_t )pVisitBlock && pVisitBlock != pxEnd ) {
                    pFreeBlock->nextBlock = pVisitBlock->nextBlock;
                    pFreeBlock->size += ( xBlockStructSize + ( pVisitBlock->size ) );
                }
                else {
                    pFreeBlock->nextBlock = pVisitBlock;
                }

                //檢查釋放空間頭部是否剛好連接鏈表，如果連接，將他們合成。
                if ( ( ( size_t )pPreviousVisitBlock + xBlockStructSize + pPreviousVisitBlock->size ) == ( size_t )pFreeBlock ) {
                    pPreviousVisitBlock->nextBlock = pFreeBlock->nextBlock;
                    pPreviousVisitBlock->size += ( xBlockStructSize + ( pFreeBlock->size ) );
                }
                else {
                    pPreviousVisitBlock->nextBlock = pFreeBlock;
                }
                break;
            }
            pPreviousVisitBlock = pVisitBlock;
            pVisitBlock         = pVisitBlock->nextBlock;
        }
    }
}

/**
 * @brief  取得分配大小
 * @note   取得最終的分配大小，而非申請大小，數值可能因記憶體對齊而有所差異，差距最大為(VM_ALIGNMENT_SIZE - 1)
 * @param  pr: 申請的指標
 * @retval 回傳大小
 */
size_t vMemSizeOf( void* pr ) {
    stBlock_T* linker = ( stBlock_T* )( ( size_t )pr - xBlockStructSize );
    return linker->size;
}
/**
 * @brief  複製記憶體，功能等同於memcpy
 * @note
 * @param  dest: 目標記憶體位址
 * @param  src: 被複製記憶體位址
 * @param  len: 複製長度
 * @retval None
 */
void vMemCopy( void* dest, const void* src, size_t len ) {
    char*       d = dest;
    const char* s = src;
    while ( len-- ) { *d++ = *s++; }
}
/**
 * @brief  打印記憶體分配情況
 * @note
 * @retval None
 */
void vMemInfoPrint( void ) {
    stBlock_T* pVisitBlock = pxStart.nextBlock;
    size_t     uAddr       = ( size_t )memArr;
    size_t     blockSize;
    size_t     c_memAddr;

    if ( pxEnd == NULL ) {
        __PRINTF( "Memory is not initialized\n" );
        return;
    }

    if ( ( uAddr & VM_ALIGNMENT_MASK ) != 0 ) {
        uAddr += ( size_t )( VM_ALIGNMENT_SIZE - 1 );
        uAddr &= ~( ( size_t )( VM_ALIGNMENT_MASK ) );
    }
    blockSize = ( ( size_t )pxEnd - uAddr ) + xBlockStructSize;

    __PRINTF( "***************************************************\n" );
    __PRINTF( "Array Address : 0x%X ; Size : 0x%X(%d)\n", ( size_t )memArr, VM_MEM_SIZE, VM_MEM_SIZE );
    __PRINTF( "Alignment Array Address : 0x%X\n", uAddr );
    __PRINTF( "Initialization Free Size : 0x%X(%d)\n", blockSize, blockSize );
    __PRINTF( "***************************************************\n" );
    __PRINTF( "Free Size : 0x%X(%d).....%0.1f%%\n", FreeSize, FreeSize, ( ( float )FreeSize / blockSize ) * 100 );
    __PRINTF( "Used space Size : 0x%X(%d).....%0.1f%%\n", ( blockSize - FreeSize ), ( blockSize - FreeSize ), ( ( float )( blockSize - FreeSize ) / blockSize ) * 100 );
    __PRINTF( "***************************************************\n" );
    pVisitBlock          = &pxStart;
    size_t blockTailAddr = ( size_t )pVisitBlock->nextBlock;
    if ( blockTailAddr > uAddr ) {
        size_t dataBlockSize = blockTailAddr - uAddr;
        __PRINTF( "=========================================\n" );
        __PRINTF( "Data Block Address : 0x%X\n", uAddr );
        __PRINTF( "Block Size : 0x%X(%d).....%0.1f%%\n", dataBlockSize, dataBlockSize, ( ( float )dataBlockSize / blockSize ) * 100 );
    }
    pVisitBlock = pVisitBlock->nextBlock;
    while ( pVisitBlock != NULL ) {
        size_t size = pVisitBlock->size + xBlockStructSize;
        __PRINTF( "=========================================\n" );
        __PRINTF( "Free Block Address : 0x%X\n", ( size_t )pVisitBlock );
        __PRINTF( "Block Size : 0x%X(%d).....%0.1f%%\n", size, size, ( ( float )size / blockSize ) * 100 );
        __PRINTF( "Next Block Address : 0x%X\n", ( size_t )pVisitBlock->nextBlock );

        blockTailAddr = ( size_t )pVisitBlock + ( ( pVisitBlock->size ) + xBlockStructSize );
        if ( ( size_t )( pVisitBlock->nextBlock ) > blockTailAddr ) {
            size_t dataBlockSize = ( size_t )( pVisitBlock->nextBlock ) - blockTailAddr;
            __PRINTF( "=========================================\n" );
            __PRINTF( "Data Block Address : 0x%X\n", blockTailAddr );
            __PRINTF( "Block Size : 0x%X(%d).....%0.1f%%\n", dataBlockSize, dataBlockSize, ( ( float )dataBlockSize / blockSize ) * 100 );
        }
        pVisitBlock = pVisitBlock->nextBlock;
    }
    __PRINTF( "=========================================\n" );
    c_memAddr   = uAddr;
    pVisitBlock = pxStart.nextBlock;
    __PRINTF( "Allocated : [%c]    ", '#' );
    __PRINTF( "Linker : [%c]   ", '@' );
    __PRINTF( "Free Block : [%c]\n", ':' );
    while ( pVisitBlock != NULL ) {
        size_t i;
        while ( c_memAddr < ( size_t )pVisitBlock ) {
            __PRINTF( "#" );
            c_memAddr++;
        }
        for ( i = 0; i < xBlockStructSize; i++ ) {
            __PRINTF( "@" );
            c_memAddr++;
        }
        for ( i = 0; i < pVisitBlock->size; i++ ) {
            __PRINTF( ":" );
            c_memAddr++;
        }
        pVisitBlock = pVisitBlock->nextBlock;
    }
    __PRINTF( "\n" );
}
