#include "entity_types.h"

#include "xtea.h"


// server key 在配置里做
char server_key[auth_service_key_length] = { 'w', 'e', 'b', '@', '5', 'd', 'e', 'z', 'o', 'n', 'e', '.', 'c', 'o', 'm', '.'};

int decrypt_signature(char* in, int16_t in_length, signature_t& signature)
{
    if ((NULL == in) || (in_length <= 0))
    {
        return -1;
    }
    
    char uncrypt_signature[max_signature_length] = {'\0',};
    int out_length = 0; 
    utils::XTEA::decipher((char*)server_key, (char*)in, (size_t)in_length,
                           uncrypt_signature, sizeof(uncrypt_signature), out_length);

    if ((0 >= out_length) || (out_length > max_signature_length))
    {
        return -1;
    }

    // decode signature
    int16_t decoded_len = 0;
    // 得到与client通讯时加密密钥 m_key_
    memcpy(signature.m_session_key_, uncrypt_signature + decoded_len, crypt_key_length);
    decoded_len += crypt_key_length;

    // 得到用户的服务标识信息  m_service_bitmap_
    memcpy(signature.m_service_bitmap_, uncrypt_signature + decoded_len, server_bitmap_length);
    decoded_len += server_bitmap_length;

    // 得到用户 m_uin_
    signature.m_client_uin_ = ntohl(*(uint32_t*)(uncrypt_signature + decoded_len));
    decoded_len += sizeof(signature.m_client_uin_);

    // 得到签名的时间戳 m_timestamp_
    signature.m_timestamp_ = ntohl(*(uint32_t*)(uncrypt_signature + decoded_len));
    decoded_len += sizeof(signature.m_timestamp_);

    if (decoded_len > out_length)
    {
        return -1;
    }

    return 0;
}

int encrypt_signature(const signature_t& signature, char* out, int16_t& out_length)
{
    if ((NULL == out) || (out_length <= 0))
    {
        return -1;
    }

    char uncrypt_signature[max_signature_length] = {'\0',};
    // encode signature
    int16_t encoded_len = 0;
    // 得到与client通讯时加密密钥 m_key_
    memcpy(uncrypt_signature + encoded_len, signature.m_session_key_, crypt_key_length);
    encoded_len += crypt_key_length;

    // 得到用户的服务标识信息  m_service_bitmap_
    memcpy(uncrypt_signature + encoded_len, signature.m_service_bitmap_, server_bitmap_length);
    encoded_len += server_bitmap_length;

    // 得到用户 m_uin_
    *(uint32_t*)(uncrypt_signature + encoded_len) = htonl(signature.m_client_uin_);
    encoded_len += sizeof(signature.m_client_uin_);

    // 得到签名的时间戳 m_timestamp_
    *(uint32_t*)(uncrypt_signature + encoded_len) = htonl(signature.m_timestamp_);
    encoded_len += sizeof(signature.m_timestamp_);

    if (encoded_len > sizeof(uncrypt_signature))
    {
        return -1;
    }

    int out_len = 0; 
    utils::XTEA::encipher((char*)server_key, uncrypt_signature, sizeof(uncrypt_signature),
                          (char*)out, (size_t)out_length, out_len);

    if ((0 >= out_len) || (out_len > max_signature_length))
    {
        return -1;
    }

    out_length = out_len;

    return 0;
}


int encrypt_buffer(const char* key, const char* in, const int32_t in_length,
                   char* out, int32_t& out_length)
{
    if ((NULL == key) || (NULL == in) || (in_length <= 0) || (NULL == out) || (out_length <= 0))
    {
        return -1;
    }

    if (out_length < in_length + 16)
    {
        return -1;
    }

    // 用session key来加密
    int out_len = 0; 
    utils::XTEA::encipher((char*)(key), (char*)(in), (size_t)(in_length),
                           out, (size_t)(out_length), out_len);
    if (0 == out_len)
    {
        return -1;
    }

    out_length = out_len;

    return 0;
}


int decrypt_buffer(const char* key, const char* in, const int32_t in_length,
                   char* out, int32_t& out_length)
{
    if ((NULL == key) || (NULL == in) || (in_length <= 0) || (NULL == out) || (out_length <= 0))
    {
        return -1;
    }

    // 用session key来解密
    int out_len = 0; 
    utils::XTEA::decipher((char*)(key), (char*)(in), (size_t)(in_length),
                           out, (size_t)(out_length), out_len);
    if (0 == out_len)
    {
        return -1;
    }

    out_length = out_len;

    return 0;
}



template<>
template<>
void binary_input_packet<true>::get_head<net_hdr_t>(net_hdr_t& value)
{
    if (sizeof(net_hdr_t) != this->m_head_len_)
    {
        this->m_good_ = false;
        return;
    }

    value = *reinterpret_cast<net_hdr_t*>(this->m_begin_);
    value.m_packet_len_ = ntohl(value.m_packet_len_);
    value.m_message_id_ = ntohs(value.m_message_id_);
    value.m_message_type_ = ntohs(value.m_message_type_);
    value.m_reserved_ = ntohl(value.m_reserved_);

    value.m_request_sequence_ = ntohl(value.m_request_sequence_);
    value.m_control_type_ = ntohs(value.m_control_type_);
    value.m_client_net_id_ = ntohl(value.m_client_net_id_);
    value.m_client_uin_ = ntohl(value.m_client_uin_);
    value.m_from_uin_ = ntohs(value.m_from_uin_);
    value.m_to_uin_ = ntohs(value.m_to_uin_);
}

template<>
template<>
void binary_output_packet<true>::set_head<net_hdr_t>(net_hdr_t value)
{
    if (sizeof(net_hdr_t) != this->m_head_len_)
    {
        this->m_good_ = false;
        return;
    }

    value.m_packet_len_ = htonl(value.m_packet_len_);
    value.m_message_id_ = htons(value.m_message_id_);
    value.m_message_type_ = htons(value.m_message_type_);
    value.m_reserved_ = htonl(value.m_reserved_);

    value.m_request_sequence_ = htonl(value.m_request_sequence_);
    value.m_control_type_ = htons(value.m_control_type_);
    value.m_client_net_id_ = htonl(value.m_client_net_id_);
    value.m_client_uin_ = htonl(value.m_client_uin_);
    value.m_from_uin_ = htons(value.m_from_uin_);
    value.m_to_uin_ = htons(value.m_to_uin_);

    *reinterpret_cast<net_hdr_t*>(this->m_begin_) = value;
}

