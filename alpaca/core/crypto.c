#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/aes.h>

// Internal
#include <core/crypto.h>
#include <core/logging.h>
#include <core/codes.h>


/**
 * FOR TESTING ONLY!!! 
 */
byte key[AES_256_KEY_SIZE] = {"12345678912345678912345678912345"};
byte iv[AES_IV_SIZE]  = {"abcdefghijklmnop"};


ALPACA_STATUS MyEncrypt(uint8_t* plain, uint32_t plainLength, uint8_t* cipher){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    int ret = 0;
    Aes enc;
    

    LOGINFO("Plain String:\n\t%s\n", plain);

    if(!plain || !cipher){
        LOGERROR("invalid param\n");
        result = ALPACA_ERROR_BADPARAM;
        goto done;
    }
   
    ret = wc_AesSetKeyDirect(&enc, key, sizeof(key), iv, AES_ENCRYPTION);
    if(ret != 0) {
        LOGERROR("Error on setkey\n");
        result = ALPACA_ERROR_BADPARAM;
        goto done;
    }

    ret = wc_AesCtrEncrypt(&enc, cipher, plain, plainLength);
    if(ret != 0){
        LOGERROR("Error on Encrypt\n");
        result = ALPACA_ERROR_BADPARAM;
        goto done;
    }

    LOGINFO("Cipher String:\n\t%s\n", cipher);



done:
    return result;
}

ALPACA_STATUS MyDecrypt(uint8_t* plain, uint32_t plainLength, uint8_t* cipher){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    int ret = 0;
    Aes dec;
    

    LOGINFO("Cipher String:\n\t%s\n", cipher);

    if(!plain || !cipher){
        result = ALPACA_ERROR_BADPARAM;
        goto done;
    }
   
    ret = wc_AesSetKeyDirect(&dec, key, sizeof(key), iv, AES_ENCRYPTION);
    if(ret != 0) {
        result = ALPACA_ERROR_BADPARAM;
        goto done;
    }

    ret = wc_AesCtrEncrypt(&dec, plain, cipher, plainLength);
    if(ret != 0){
        result = ALPACA_ERROR_BADPARAM;
        goto done;
    }

    LOGINFO("Plain String:\n\t%s\n", plain);



done:
    return result;
}


/*
wc_AesCtrEncrypt(&enc, cipher, msg, sizeof(msg)); // encrypt plain
wc_AesCtrEncrypt(&dec, decrypted, cipher, sizeof(cipher)); // decrypt cipher text
*/