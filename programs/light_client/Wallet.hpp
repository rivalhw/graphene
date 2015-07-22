#pragma once
#pragma GCC diagnostic ignored "-Wunknown-pragmas"

#include <graphene/chain/protocol/types.hpp>
#include <QObject>

using std::string;
using std::vector;
using std::set;
using fc::flat_set;
using std::map;
using graphene::chain::public_key_type;
using graphene::chain::digest_type;
using graphene::chain::signature_type;
using fc::optional;

struct key_data
{
   string       label; /** unique label assigned to this key */
   vector<char> encrypted_private_key;
};
FC_REFLECT( key_data, (label)(encrypted_private_key) );

struct wallet_file
{
   vector<char>                    encrypted_brain_key;
   fc::sha512                      brain_key_digest;
   vector<char>                    encrypted_master_key;
   fc::sha512                      master_key_digest;
   map<public_key_type, key_data>  encrypted_private_keys;
};

FC_REFLECT( wallet_file, 
            (encrypted_brain_key)
            (brain_key_digest)
            (encrypted_master_key)
            (master_key_digest)
            (encrypted_private_keys) 
          );


/**
 *  @class Wallet
 *  @brief Securely maintains a set of labeled public and private keys
 */
class Wallet : public QObject
{
   public:
      Q_OBJECT

      Wallet();
      ~Wallet();

      Q_INVOKABLE bool open( QString file_path );
      Q_INVOKABLE bool close();
      Q_INVOKABLE bool isOpen()const;
      Q_INVOKABLE bool save();
      Q_INVOKABLE bool saveAs( QString file_path );
      Q_INVOKABLE bool create( QString file_path, QString password, QString brain_key = QString() );

      /** required to generate new owner keys */
      Q_INVOKABLE bool loadBrainKey( QString brain_key );

      /** removes brain key to secure owner keys */
      Q_INVOKABLE bool purgeBrainKey();
      Q_INVOKABLE bool hasBrainKey()const;

      /** @pre hasBrainKey() */
      Q_INVOKABLE QString getBrainKey();

      Q_INVOKABLE bool isLocked()const;
      Q_INVOKABLE bool unlock( QString password );
      Q_INVOKABLE bool lock();
      Q_INVOKABLE bool changePassword( QString new_password );

      /**
       * @pre !isLocked();
       * @post save()
       * @return WIF private key 
       */
      Q_INVOKABLE QString getActivePrivateKey( QString owner_public_key, uint32_t sequence );

      /**
       * @pre !isLocked();
       * @pre hasBrainKey();
       * @post save()
       * @return WIF private key 
       */
      Q_INVOKABLE QString getOwnerPrivateKey( uint32_t sequence );

      Q_INVOKABLE QString getKeyLabel( QString pubkey );
      Q_INVOKABLE QString getPublicKey( QString label );

      /** imports a public key and assigns it a label */
      Q_INVOKABLE bool    importPublicKey( QString pubkey, QString label = QString() );

      /** 
       * @param wifkey a private key in (WIF) Wallet Import Format
       * @pre !isLocked() 
       **/
      Q_INVOKABLE bool    importPrivateKey( QString wifkey, QString label = QString() );

      /** removes the key, its lablel and its private key */
      Q_INVOKABLE bool    removePublicKey( QString pubkey );

      /** removes only the private key, keeping the public key and label */
      Q_INVOKABLE bool    removePrivateKey( QString pubkey );


      /**
       * @pre !isLocked()
       */
      vector<signature_type>           signDigest( const digest_type& d, 
                                                   const set<public_key_type>& keys )const;

      const flat_set<public_key_type>& getAvailablePrivateKeys()const;

   private:
      fc::path                  _wallet_file_path;
      wallet_file               _data;
      /** used to decrypt all of the encrypted private keys */
      fc::sha512                _decrypted_master_key;
      flat_set<public_key_type> _available_private_keys;
      map<QString,QString>      _label_to_key;
      QString                   _brain_key;
};


