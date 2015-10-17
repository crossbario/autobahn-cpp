
#ifndef WAMP_AUTH_UTILS_HPP
#define WAMP_AUTH_UTILS_HPP


#define USING_BOTAN_CRYPTO


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <exception>

// 
// Execpetion thrown when something gets wrong 
// creating the derived auth key.....
//
class derived_key_error : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Error occured when calulcate a derived key";
  }
};



#ifdef USING_BOTAN_CRYPTO 
//////////////////////////////////////////////////////
// - using botan crypto lib
// see http://botan.randombit.net/
//////////////////////////////////////////////////////


#include <botan/lookup.h>
#include <botan/hex.h>
#include <botan/base64.h>

#include <botan/sha2_32.h>
#include <botan/hmac.h>


/*!
 * create a derived key from a password/secret 
 *
 * \param  passwd A secret string to make a derived key for
 * \param  salt A random salt added to the key 
 * \param  iterations A number of intertions used to create the derived key 
 * \param  keylen The length of the derived key returned. 
 * \return a PBKDF2-sha256 derived key 
 */
inline std::string derive_key(
        const std::string & passwd,
        const std::string & salt,
        int iterations,
        int keylen
		)
{
    using namespace Botan;

    PBKDF* pbkdf = get_pbkdf("PBKDF2(SHA-256)");

    OctetString key = pbkdf->derive_key(
            keylen, 
            passwd,
            ( const unsigned char * ) &salt[0],
            salt.size(),
            iterations);

    return base64_encode( hex_decode( key.as_string() ) );
}


/*!
 * make a keyed-hash from a key using the HMAC-sha256 and a challenge
 *
 * \param key The key to make a digest for 
 * \param challenge Some data mixin - identify the specific digest 
 * \return a base64 encoded digest  
 */
inline std::string compute_wcs(
        const std::string & key,
        const std::string & challenge )
{
    using namespace Botan;

    SHA_256 * _hash = new SHA_256(); 
    // remark! SHA_256 is deleted by the HMAC in its destructor
    
    Botan::HMAC _mac( _hash );

    _mac.set_key( ( const byte * ) key.data() , key.size() );
    _mac.update( challenge );

    auto f = _mac.final();
    std::string b = base64_encode( f );

    return b;
}

#endif //USING_BOTAN_CRYPTO


#ifdef USING_OPENSSL_CRYPTO 
//////////////////////////////////////////////////////
// - using openssl crypto lib
// see openssl at : https://www.openssl.org
//////////////////////////////////////////////////////


#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include <stdint.h>


/*!
 * base64 encoding
 *
 * \param data The data to be encoded 
 * \return A encoded string 
 */
inline std::string base_64_encode(const std::string & data )
{ 
    BIO *bio, *b64;
    BUF_MEM *pBuf;
    
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); 
    
    BIO_write(bio, (const unsigned char *) data.c_str(), data.size());
    BIO_flush(bio);
    
    BIO_get_mem_ptr(bio, &pBuf);
    BIO_set_close(bio, BIO_NOCLOSE);
    
    std::string str_out;
    str_out.assign( pBuf->data, pBuf->length );
    
    BIO_free_all(bio);
    
    return str_out;
}


/*!
 * create a derived key from a password/secret 
 *
 * \param  passwd A secret string to make a derived key for
 * \param  salt A random salt added to the key 
 * \param  iterations A number of intertions used to create the derived key 
 * \param  keylen The length of the derived key returned. 
 * \return a PBKDF2-sha256 derived key 
 */
inline std::string derive_key(
        const std::string & passwd,
        const std::string & salt,
        int iterations,
        int keylen
		)
{

    int passwdLen = passwd.size();
    const char * pwd = passwd.c_str();

    int saltLen = salt.size();
    unsigned char * salt_value = (unsigned char * ) salt.c_str();

    std::string str_out;
    str_out.resize( keylen );
	

    unsigned char * out = (unsigned char *) str_out.c_str();


    int result = PKCS5_PBKDF2_HMAC(
		    pwd, passwdLen, 
		    salt_value, saltLen, 
		    iterations, 
		    EVP_sha256(),
		    keylen, out);

    if ( result != 0 )
    {
	    return base_64_encode( str_out );
    }
    else
    { 
	    throw derived_key_error();
    }
}


/*!
 * make a keyed-hash from a key using the HMAC-sha256 and a challenge
 *
 * \param key The key to make a digest for 
 * \param challenge Some data mixin - identify the specific digest 
 * \return a base64 encoded digest  
 */
inline std::string compute_wcs(
        const std::string & key,
        const std::string & challenge )
{

    unsigned int len = 32;
    unsigned char hash[32];

    HMAC_CTX hmac;
    HMAC_CTX_init(&hmac);
    HMAC_Init_ex(&hmac, key.data(), key.length(), EVP_sha256(), NULL);
    HMAC_Update(&hmac, ( unsigned char* ) challenge.data(), challenge.length());
    HMAC_Final(&hmac, hash, &len);
    HMAC_CTX_cleanup(&hmac);

    
    std::string str_out;
    str_out.assign( ( char * ) &hash , 32 );

    return base_64_encode( str_out );
}

#endif //USING_OPENSSL_CRYPTO


#endif //WAMP_AUTH_UTILS_HPP
