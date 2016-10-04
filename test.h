#ifndef __OCB__H
#define __OCB__H

#ifndef AES_KEY_BITLEN
#define AES_KEY_BITLEN   128  /* Must be 128, 192, 256 */
#endif

#if ((AES_KEY_BITLEN != 128) && \
     (AES_KEY_BITLEN != 192) && \
     (AES_KEY_BITLEN != 256))
#error Bad -- AES_KEY_BITLEN must be one of 128, 192 or 256!!
#endif
#ifndef __RIJNDAEL_ALG_FST_H
#define __RIJNDAEL_ALG_FST_H

#define MAXKC	(256/32)
#define MAXKB	(256/8)
#define MAXNR	14

typedef unsigned char	u8;	
typedef unsigned short	u16;	
typedef unsigned int	u32;

int rijndaelKeySetupEnc(u32 rk[/*4*(Nr + 1)*/], const u8 cipherKey[], int keyBits);
int rijndaelKeySetupDec(u32 rk[/*4*(Nr + 1)*/], const u8 cipherKey[], int keyBits);
void rijndaelEncrypt(const u32 rk[/*4*(Nr + 1)*/], int Nr, const u8 pt[16], u8 ct[16]);
void rijndaelDecrypt(const u32 rk[/*4*(Nr + 1)*/], int Nr, const u8 ct[16], u8 pt[16]);

#ifdef INTERMEDIATE_VALUE_KAT
void rijndaelEncryptRound(const u32 rk[/*4*(Nr + 1)*/], int Nr, u8 block[16], int rounds);
void rijndaelDecryptRound(const u32 rk[/*4*(Nr + 1)*/], int Nr, u8 block[16], int rounds);
#endif /* INTERMEDIATE_VALUE_KAT */

#endif /* __RIJNDAEL_ALG_FST_H */
typedef struct _keystruct keystruct;

/*
 * "ocb_aes_init" optionally creates an ocb keystructure in memory
 * and then initializes it using the supplied "enc_key". "tag_len"
 * specifies the length of tags that will subsequently be generated
 * and verified. If "key" is NULL a new structure will be created, but
 * if "key" is non-NULL, then it is assumed that it points to a previously
 * allocated structure, and that structure is initialized. "ocb_aes_init"
 * returns a pointer to the initialized structure, or NULL if an error
 * occurred.
 */
keystruct *                         /* Init'd keystruct or NULL      */
ocb_aes_init(void      *enc_key,    /* AES key                       */
             unsigned   tag_len,    /* Length of tags to be used     */
             keystruct *key);       /* OCB key structure. NULL means */
                                    /* Allocate/init new, non-NULL   */
                                    /* means init existing structure */

/* "ocb_done deallocates a key structure and returns NULL */
keystruct *
ocb_done(keystruct *key);
                              
/*
 * "ocb_aes_encrypt takes a key structure, four buffers and a length
 * parameter as input. "pt_len" bytes that are pointed to by "pt" are
 * encrypted and written to the buffer pointed to by "ct". A tag of length
 * "tag_len" (set in ocb_aes_init) is written to the "tag" buffer. "nonce"
 * must be a 16-byte buffer which changes for each new message being
 * encrypted. "ocb_aes_encrypt" always returns a value of 1.
 */
void
ocb_aes_encrypt(keystruct *key,    /* Initialized key struct           */
                void      *nonce,  /* 16-byte nonce                    */
                void      *pt,     /* Buffer for (incoming) plaintext  */
                unsigned   pt_len, /* Byte length of pt                */
                void      *ct,     /* Buffer for (outgoing) ciphertext */
                void      *tag);   /* Buffer for generated tag         */

                              
/*
 * "ocb_aes_decrypt takes a key structure, four buffers and a length
 * parameter as input. "ct_len" bytes that are pointed to by "ct" are
 * decrypted and written to the buffer pointed to by "pt". A tag of length
 * "tag_len" (set in ocb_aes_init) is read from the "tag" buffer. "nonce"
 * must be a 16-byte buffer which changes for each new message being
 * encrypted. "ocb_aes_decrypt" returns 0 if the supplied
 * tag is not correct for the supplied message, otherwise 1 is returned if
 * the tag is correct.
 */
int                                /* Returns 0 iff tag is incorrect   */
ocb_aes_decrypt(keystruct *key,    /* Initialized key struct           */
                void      *nonce,  /* 16-byte nonce                    */
                void      *ct,     /* Buffer for (incoming) ciphertext */
                unsigned   ct_len, /* Byte length of ct                */
                void      *pt,     /* Buffer for (outgoing) plaintext  */
                void      *tag);   /* Tag to be verified               */
                     



void
pmac_aes (keystruct *key,    /* Initialized key struct           */
          void      *in,     /* Buffer for (incoming) message    */
          unsigned   in_len, /* Byte length of message           */
          void      *tag);    /* 16-byte buffer for generated tag */

#endif /* __OCB__H */
