/**	@file card.h
 *	@brief The CARD API provides file level access to memory cards.
 *
 *	Refer to the dolphin OS Reference Manual for further information
 *
 *	@author AECX
 *	@bug No known bugs.
 */
#ifndef GC_WII_CARD_H
#define GC_WII_CARD_H

#include "gc_wii/dsp.h"
#include "gc_wii/OSThead.h"
#include "gc_wii/OSAlarm.h"
#include "gc_wii/dvd.h"

#include <cstdint>

#define CARD_SLOT_A 0
#define CARD_SLOT_B 1

#define CARD_BLOCK_SIZE (8 * 1024)
#define CARD_WORKAREA_SIZE (5 * 8 * 1024)
#define CARD_MAX_FILE 127

#define CARD_ENCODE_ANSI 0u
#define CARD_ENCODE_SJIS 1u

// longest file name string excluding terminating zero
#define CARD_FILENAME_MAX 32

#define CARD_ICON_MAX 8
#define CARD_ICON_WIDTH 32
#define CARD_ICON_HEIGTH 32
#define CARD_BANNER_WIDTH 96
#define CARD_BANNER_HEIGTH 32

#define CARD_RESULT_UNLOCKED 1
#define CARD_RESULT_READY 0
#define CARD_RESULT_BUSY -1
#define CARD_RESULT_WRONGDEVICE -2
#define CARD_RESULT_NOCARD -3
#define CARD_RESULT_NOFILE -4
#define CARD_RESULT_IOERROR -5
#define CARD_RESULT_BROKEN -6
#define CARD_RESULT_EXIST -7
#define CARD_RESULT_NOENT -8
#define CARD_RESULT_INSSPACE -9
#define CARD_RESULT_NOPERM -10
#define CARD_RESULT_LIMIT -11
#define CARD_RESULT_NAMETOOLONG -12
#define CARD_RESULT_ENCODING -13
#define CARD_RESULT_FATAL_ERROR -128

#define CARD_READ_SIZE 512

// Taken from https://github.com/TakaRikka/dolsdk2004/blob/card/include/dolphin/card.h
#define CARD_NUM_SYSTEM_BLOCK 5

namespace libtp::gc_wii::card
{
    typedef void (*CARDCallback)(int32_t chan, int32_t result);

    struct CARDFileInfo
    {
        int32_t chan;
        int32_t fileNo;
        int32_t offset;
        int32_t length;
        uint16_t iBlock;
        uint16_t padding;
    } __attribute__((__packed__));

    struct CARDStat
    {
        // read-only (Set by CARDGetStatus)
        char fileName[CARD_FILENAME_MAX];
        uint32_t length;
        uint32_t time;
        uint8_t gameName[4];
        uint8_t company[2];
        uint8_t gameVersion;

        // read/write
        uint8_t bannerFormat;
        uint32_t iconAddr;
        uint16_t iconFormat;
        uint16_t iconSpeed;
        uint32_t commentAddr;

        // read-only (Set by CARDSetStatus/CARDGetStatus)
        uint32_t offsetBanner;
        uint32_t offsetBannerTlut;
        uint32_t offsetIcon[CARD_ICON_MAX];
        uint32_t offsetIconTlut;
        uint32_t offsetData;
    } __attribute__((__packed__));

    // Taken from https://github.com/TakaRikka/dolsdk2004/blob/card/include/dolphin/card.h
    struct CARDDir
    {
        /*0x00*/ uint8_t gameName[4];
        /*0x04*/ uint8_t company[2];
        /*0x06*/ uint8_t _padding0;
        /*0x07*/ uint8_t bannerFormat;
        /*0x08*/ uint8_t fileName[CARD_FILENAME_MAX];
        /*0x28*/ uint32_t time;     // seconds since 01/01/2000 midnight
        /*0x2C*/ uint32_t iconAddr; // 0xffffffff if not used
        /*0x30*/ uint16_t iconFormat;
        /*0x32*/ uint16_t iconSpeed;
        /*0x34*/ uint8_t permission;
        /*0x35*/ uint8_t copyTimes;
        /*0x36*/ uint16_t startBlock;
        /*0x38*/ uint16_t length;
        /*0x3A*/ uint8_t _padding1[2];
        /*0x3C*/ uint32_t commentAddr; // 0xffffffff if not used
    } __attribute__((__packed__));

    struct CARDControl
    {
        /* 0x000 */ int32_t attached;
        /* 0x004 */ int32_t result;
        /* 0x008 */ uint16_t size;
        /* 0x00A */ uint16_t pageSize;
        /* 0x00C */ int32_t sectorSize;
        /* 0x010 */ uint16_t cBlock;
        /* 0x012 */ uint16_t vendorID;
        /* 0x014 */ int32_t latency;
        /* 0x018 */ uint8_t id[12];
        /* 0x024 */ int32_t mountStep;
        /* 0x028 */ int32_t formatStep;
        /* 0x02C */ uint32_t scramble;
        /* 0x030 */ dsp::DSPTaskInfo task;
        /* 0x080 */ void* workArea;
        /* 0x084 */ CARDDir* currentDir;
        /* 0x088 */ uint16_t* currentFat;
        /* 0x08C */ os_thread::OSThreadQueue threadQueue;
        /* 0x094 */ uint8_t cmd[9];
        /* 0x09D */ uint8_t padding_9d[3];
        /* 0x0A0 */ int32_t cmdlen;
        /* 0x0A4 */ volatile uint32_t mode;
        /* 0x0A8 */ int32_t retry;
        /* 0x0AC */ int32_t repeat;
        /* 0x0B0 */ uint32_t addr;
        /* 0x0B4 */ void* buffer;
        /* 0x0B8 */ int32_t xferred;
        /* 0x0BC */ uint16_t freeNo;
        /* 0x0BE */ uint16_t startBlock;
        /* 0x0C0 */ CARDFileInfo* fileInfo;
        /* 0x0C4 */ CARDCallback extCallback;
        /* 0x0C8 */ CARDCallback txCallback;
        /* 0x0CC */ CARDCallback exiCallback;
        /* 0x0D0 */ CARDCallback apiCallback;
        /* 0x0D4 */ CARDCallback xferCallback;
        /* 0x0D8 */ CARDCallback eraseCallback;
        /* 0x0DC */ CARDCallback unlockCallback;
        /* 0x0E0 */ os_alarm::OSAlarm alarm;
        /* 0x108 */ uint32_t cid;
        /* 0x10C */ const dvd::DVDDiskID* diskID;
    } __attribute__((__packed__));

    static_assert(sizeof(CARDFileInfo) == 0x14);
    static_assert(sizeof(CARDStat) == 0x6C);
    static_assert(sizeof(CARDDir) == 0x40);
    static_assert(sizeof(CARDControl) == 0x110);

    extern "C"
    {
        // Variables
        /**
         *  @brief No decent description at this time
         */
        extern CARDControl __CARDBlock[2]; // One for each memory card slot

        // Functions
        /**
         *  @brief Initializes memory card API control blocks. Must be called once before using any other CARD API functions.
         */
        void CARDInit(void);

        /**
         *  @brief Checks for the file system consistency of the memory card and repair it if possible.
         *
         *  @param chan EXI channel number
         */
        int32_t CARDCheck(int32_t chan);

        // int32_t CARDCheckAsync( int32_t chan, CARDCallback callback );
        // int32_t CARDCreate( int32_t chan, char* fileName, u32 size, CARDFileInfo* fileInfo );
        // int32_t CARDCreateAsync( int32_t chan, char* fileName, u32 size, CARDFileInfo* fileInfo, CARDCallback callback );
        // int32_t CARDDelete( int32_t chan, char* fileName );
        // int32_t CARDDeleteAsync( int32_t chan, char* fileName, CARDCallback callback );
        // int32_t CARDFastDelete( int32_t chan, int32_t fileNo );
        // int32_t CARDFastDeleteAsync( int32_t chan, int32_t fileNo, CARDCallback callback );
        // int32_t CARDFastOpen( int32_t chan, int32_t fileNo, CARDFileInfo* fileInfo );
        // int32_t CARDFormat( int32_t chan );
        // int32_t CARDFormatAsync( int32_t chan, CARDCallback callback );
        // int32_t CARDFreeBlocks( int32_t chan, int32_t* byteNotUsed, int32_t* filesNotUsed );
        // int32_t CARDGetResultCode( int32_t chan );
        // int32_t CARDGetEncoding( int32_t chan, u16* encode );
        // int32_t CARDGetStatus( int32_t chan, int32_t fileNo, CARDStat* stat );

        // int32_t CARDMountAsync( int32_t chan, void* workArea, CARDCallback detachCallback, CARDCallback attachCallback );
        /**
         *  @brief Mounts a memory card synchronously.
         *
         *  @param chan EXI channel number
         *  @param workArea pointer to the work area to be used while a memory card is in use.
         *  The work area must be 32 byte aligned and must be larger than or equal to CARD_WORKAREA_SIZE.
         *  @param detachCallback callback function to be called when the memory card is removed from the slot.
         */
        int32_t CARDMount(int32_t chan, void* workArea, CARDCallback detachCallback);

        /**
         *  @brief Unmounts a memory card Synchronously.
         *
         *  The work area specified by CARDMount() or CARDMountAsync() is also released.
         *  The function returns CARD_RESULT_READY if the memory card is successfully unmounted.
         *
         *  @param chan EXI channel number
         */
        int32_t CARDUnmount(int32_t chan);

        /**
         *  @brief Opens a file.
         *
         *  @param chan	EXI channel number
         *  @param fileName	pointer to file name to be opened
         *  @param fileInfo	pointer to file info to be used
         */
        int32_t CARDOpen(int32_t chan, const char* fileName, CARDFileInfo* fileInfo);

        // bool CARDProbe( int32_t chan );

        /**
         *  @brief This function verifies whether a memory card (of any type) is inserted in the specified slot.
         *
         *  The memory card size returned, in megabits, will be 4, 8, 16, 32, 64, or 128 Mb.
         *  The sector size returned, in bytes, will be 8, 16, 32, 64, or 128 KB. A game program must support memory cards whose
         *
         *  @param chan Slot number (0: slot A, 1: slot B).
         *  @param memSize Pointer to s32 object to store the memory card size, in megabits. Specify NULL if it is not
         * necessary.
         *  @param sectorSize Pointer to s32 object to store the memory card sector size, in bytes. Specify NULL if it is not
         * necessary.
         */
        int32_t CARDProbeEx(int32_t chan, int32_t* memSize, int32_t* sectorSize);
        // int32_t CARDRename( int32_t chan, char* oldName, char* newName );
        // int32_t CARDRenameAsync( int32_t chan, char* oldName, char* newName, CARDCallback callback );
        // int32_t CARDSetStatus( int32_t chan, int32_t fileNo, CARDStat* stat );
        // int32_t CARDSetStatusAsync( int32_t chan, int32_t fileNo, CARDStat* stat, CARDCallback callback );

        /**
         *  @brief Closes a file.
         *
         *  @param fileInfo file info of the file to be closed
         */
        int32_t CARDClose(CARDFileInfo* fileInfo);

        /**
         *  @brief Reads data from a file synchronously.
         *
         *
         *  @param fileInfo	file info of the file to be read
         *  @param buf buffer address (32 bytes aligned)
         *  @param length number of bytes to be read (multiple of CARD_READ_SIZE)
         *  @param offset file position at which to start the read (multiple of CARD_READ_SIZE)
         */
        int32_t CARDRead(CARDFileInfo* fileInfo, void* addr, int32_t length, int32_t offset);

        // int32_t CARDReadAsync( CARDFileInfo* fileInfo, void* addr, int32_t length, int32_t offset, CARDCallback callback );

        /**
         *  @brief Writes data to a file synchronously.
         *
         *  @param fileInfo	file info of the file to be written
         *  @param buf buffer address (32 bytes aligned)
         *  @param length number of bytes to write. The length must be multiple of the card sector size returned by
         * CARDGetSectorSize.
         *  @param offset file position at which to start the read. The offset must be multiple of the card sector size returned
         * by CARDGetSectorSize.
         */
        int32_t CARDWrite(CARDFileInfo* fileInfo, void* addr, int32_t length, int32_t offset);
        // int32_t CARDWriteAsync( CARDFileInfo* fileInfo, void* addr, int32_t length, int32_t offset, CARDCallback callback );

        // Internal CARD functions; Use with caution
        /**
         *  @brief Default CARDCallback function to be used when the user did not specify one
         *
         *  @param chan Slot number (0: slot A, 1: slot B).
         *  @param result Return value of the most recent CARD function (This description may be incorrect)
         */
        void __CARDDefaultApiCallback(int32_t chan, int32_t result);

        /**
         *  @brief Callback function to resume the current thread after the most resent asynchronous CARD function has finished
         * its operation
         *
         *  @param chan Slot number (0: slot A, 1: slot B).
         *  @param result Return value of the most recent CARD function (This description may be incorrect)
         */
        void __CARDSyncCallback(int32_t chan, int32_t result);

        /**
         *  @brief Gets the control block for the desired memory card
         *
         *  @param chan Slot number (0: slot A, 1: slot B).
         *  @param card Output for the pointer to the CARDBlock for the current memory card.
         */
        int32_t __CARDGetControlBlock(int32_t chan, CARDControl** card);

        /**
         *  @brief Puts the control block for the desired memory card
         *
         *  @param card Pointer to the CARDBlock for the current memory card.
         *  @param result Return value of the most recent CARD function.
         */
        int32_t __CARDPutControlBlock(CARDControl* card, int32_t result);

        /**
         *  @brief Suspends the current thread until the most resent asynchronous CARD function has finished its operation
         *
         *  @param chan Slot number (0: slot A, 1: slot B).
         */
        int32_t __CARDSync(int32_t chan);

        /**
         *  @brief Updates the fat block section for the specified memory card (This description may be incorrect)
         *
         *  @param chan Slot number (0: slot A, 1: slot B).
         *  @param fat Fat block section to be updated
         *  @param callback Callback function
         */
        int32_t __CARDUpdateFatBlock(int32_t chan, uint16_t* fat, CARDCallback callback);

        /**
         *  @brief Gets the pointer to the fat block section for the current memory card.
         *
         *  @param card Pointer to the CARDBlock for the current memory card.
         */
        inline uint16_t* __CARDGetFatBlock(CARDControl* card)
        {
            return card->currentFat;
        }

        /**
         *  @brief Verifies if a block is valid on the memory card (This description may be incorrect)
         *
         *  @param card Pointer to the CARDBlock for the current memory card.
         *  @param blockNo Block number to check.
         */
        inline bool CARDIsValidBlockNo(CARDControl* card, uint16_t blockNo)
        {
            return (blockNo >= CARD_NUM_SYSTEM_BLOCK) && (blockNo < card->cBlock);
        }

        /**
         *  @brief Gets the directly block for a specified file on the memory card.
         *
         *  @param card Pointer to the CARDBlock for the current memory card.
         */
        CARDDir* __CARDGetDirBlock(CARDControl* card);

        /**
         *  @brief Updates the directly block for a specified file on the memory card.
         *
         *  @param chan Slot number (0: slot A, 1: slot B).
         *  @param callback Callback function
         */
        int32_t __CARDUpdateDir(int32_t chan, CARDCallback callback);

        /**
         *  @brief Determines if the file name of the file in the current directly block is the same as the fileName parameter
         *
         *  @param dir Pointer the directly block for a specified file on the memory card.
         *  @param fileName Internal name for the desired file on the memory card
         */
        bool __CARDCompareFileName(CARDDir* dir, const char* fileName);

        /**
         *  @brief Determines if the current file on the memory card can be accessed
         *
         *  @param card Pointer to the CARDBlock for the current memory card.
         *  @param dir Pointer the directly block for a specified file on the memory card.
         */
        int32_t __CARDAccess(CARDControl* card, CARDDir* dir);

        /**
         *  @brief Gets the status of a file.
         *
         *  @param chan EXI channel number
         *  @param fileNo Index to the desired file on the memory card
         *  @param stat Output for the status of the file
         */
        int32_t CARDGetStatus(int32_t chan, int32_t fileNo, CARDStat* stat);
    }

    // Manually written functions
    /**
     *  @brief Frees specific blocks in the fat block section for the current memory card (This description may be incorrect)
     *
     *  @param chan Slot number (0: slot A, 1: slot B).
     *  @param nBlock Starting block
     *  @param Callback Callback function
     */
    int32_t __CARDFreeBlock(int32_t chan, uint16_t nBlock, CARDCallback callback);

    /**
     *  @brief Callback to be called when a file is ready to be deleted (This description may be incorrect)
     *
     *  @param chan Slot number (0: slot A, 1: slot B).
     *  @param result Return value of the most recent CARD function (This description may be incorrect)
     */
    void DeleteCallback(int32_t chan, int32_t result);

    /**
     *  @brief Gets the file number of the desired file on the memory card
     *
     *  @param card Pointer to the CARDBlock for the current memory card.
     *  @param fileName Internal name for the desired file on the memory card
     *  @param pfileNo Output for the index to the desired file on the memory card
     */
    int32_t __CARDGetFileNo(CARDControl* card, const char* fileName, int32_t* pfileNo);

    /**
     *  @brief Deletes a file asynchronously
     *
     *  @param chan Slot number (0: slot A, 1: slot B).
     *  @param fileName Internal name for the desired file on the memory card
     *  @param callback Callback function to be notified of the result
     */
    int32_t CARDDeleteAsync(int32_t chan, const char* fileName, CARDCallback callback);

    /**
     *  @brief Deletes a file synchronously
     *
     *  @param chan Slot number (0: slot A, 1: slot B).
     *  @param fileName Internal name for the desired file on the memory card
     */
    int32_t CARDDelete(int32_t chan, const char* fileName);

} // namespace libtp::gc_wii::card

#endif