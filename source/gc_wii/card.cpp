#include "gc_wii/card.h"

#include <cstring>

namespace libtp::gc_wii::card
{
    // Taken from https://github.com/TakaRikka/dolsdk2004/tree/card/include/dolphin/card
    int32_t __CARDFreeBlock(int32_t chan, uint16_t nBlock, CARDCallback callback)
    {
        CARDControl* card = &__CARDBlock[chan];
        if (!card->attached)
        {
            return CARD_RESULT_NOCARD;
        }

        uint16_t* fat = __CARDGetFatBlock(card);
        while (nBlock != 0xFFFF)
        {
            if (!CARDIsValidBlockNo(card, nBlock))
            {
                return CARD_RESULT_BROKEN;
            }

            uint16_t nextBlock = fat[nBlock];
            fat[nBlock] = 0;
            nBlock = nextBlock;
            ++fat[3];
        }

        return __CARDUpdateFatBlock(chan, fat, callback);
    }

    void DeleteCallback(int32_t chan, int32_t result)
    {
        CARDControl* card = &__CARDBlock[chan];
        CARDCallback callback = card->apiCallback;
        card->apiCallback = nullptr;

        if (result >= CARD_RESULT_READY)
        {
            result = __CARDFreeBlock(chan, card->startBlock, callback);
            if (result >= CARD_RESULT_READY)
            {
                return;
            }
        }

        __CARDPutControlBlock(card, result);
        if (callback)
        {
            callback(chan, result);
        }
    }

    int32_t __CARDGetFileNo(CARDControl* card, const char* fileName, int32_t* pfileNo)
    {
        if (!card->attached)
        {
            return CARD_RESULT_NOCARD;
        }

        CARDDir* dir = __CARDGetDirBlock(card);

        int32_t fileNo;
        for (fileNo = 0; fileNo < CARD_MAX_FILE; fileNo++)
        {
            CARDDir* ent = &dir[fileNo];

            if (__CARDAccess(card, ent) < CARD_RESULT_READY)
            {
                continue;
            }

            if (__CARDCompareFileName(ent, fileName))
            {
                *pfileNo = fileNo;
                return CARD_RESULT_READY;
            }
        }

        return CARD_RESULT_NOFILE;
    }

    int32_t CARDDeleteAsync(int32_t chan, const char* fileName, CARDCallback callback)
    {
        CARDControl* card;
        int32_t ret = __CARDGetControlBlock(chan, &card);
        if (ret < CARD_RESULT_READY)
        {
            return ret;
        }

        int32_t fileNo;
        ret = __CARDGetFileNo(card, fileName, &fileNo);
        if (ret < CARD_RESULT_READY)
        {
            __CARDPutControlBlock(card, ret);
            return ret;
        }

        CARDDir* dir = __CARDGetDirBlock(card);
        CARDDir* ent = &dir[fileNo];
        card->startBlock = ent->startBlock;
        memset(ent, 0xFF, sizeof(CARDDir));

        card->apiCallback = callback ? callback : __CARDDefaultApiCallback;
        ret = __CARDUpdateDir(chan, DeleteCallback);
        if (ret < CARD_RESULT_READY)
        {
            __CARDPutControlBlock(card, ret);
        }

        return ret;
    }

    int32_t CARDDelete(int32_t chan, const char* fileName)
    {
        int32_t ret = CARDDeleteAsync(chan, fileName, __CARDSyncCallback);
        if (ret >= CARD_RESULT_READY)
        {
            ret = __CARDSync(chan);
        }
        return ret;
    }
} // namespace libtp::gc_wii::card