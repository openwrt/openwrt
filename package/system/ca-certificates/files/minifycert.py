#!/usr/bin/env python3
import base64
import sys
import textwrap

#strip signature (make it zero-legnth bitstring) to reduce size of certificates in pem file.

def decode_pems_to_binary(pem_string: str) -> list[bytearray]:
    """
    Decodes a string that may contain multiple PEM blocks into a list of bytearrays.

    Args:
        pem_string: A string containing one or more PEM-formatted blocks.

    Returns:
        A list of bytearrays, where each bytearray is the decoded binary
        content of one PEM block. Returns an empty list if an error occurs
        or no valid PEM blocks are found.
    """
    try:
        decoded_blocks = []
        current_pem_lines = []
        in_content_block = False

        lines = pem_string.strip().split('\n')

        for line in lines:
            stripped_line = line.strip()
            if not stripped_line:
                continue # Skip empty lines

            if stripped_line.startswith('-----BEGIN'):
                # Start of a new block
                in_content_block = True
                current_pem_lines = [] # Reset for the new block
                continue

            if stripped_line.startswith('-----END') and in_content_block:
                # End of the current block, time to process it
                in_content_block = False
                
                base64_encoded_data = "".join(current_pem_lines)
                raw_binary_data = base64.b64decode(base64_encoded_data)
                decoded_blocks.append(bytearray(raw_binary_data))
                continue

            if in_content_block:
                current_pem_lines.append(stripped_line)

        return decoded_blocks

    except Exception as e:
        print(f"Error decoding PEMs to binary: {e}")
        # Returning an empty list is often better than None for functions
        # that return lists, as the caller doesn't need to check for None.
        return []

def nextasnindex(index:int, asn1_bytes:bytearray) -> int:
    """
    Given an index pointing to the first byte of an ASN.1 length field,
    returns the index of the next ASN.1 structure's length field.
    Args:
        index: The index of the first byte of the ASN.1 length field we want to skip.
        asn1_bytes: The bytearray containing the ASN.1 encoded data.
    Returns:
        The index of the next ASN.1 structure's length field.
    """
    asnlen = asn1_bytes[index]
    if asnlen >= 128:
        lenlen = asnlen-128
        i = lenlen
        asnlen = 0
        while i > 0:
            index +=1 
            asnlen = 256*asnlen + asn1_bytes[index]
            i -= 1
    else:
        lenlen = 0
    return index + asnlen + 2

# input der_cert_byte, return pem string of minified cert
def minifycert(der_cert_bytes:bytearray) -> str:
    """
    Minifies a DER-encoded certificate by stripping the signature and returning
    a PEM-formatted string of the modified certificate.
    Args:
        der_cert_bytes: A bytearray containing the DER-encoded certificate.
    Returns:
        A PEM-formatted string of the minified certificate.
    """

    # byte manipulation to follow asn1 structure
    index = 1
    if len(der_cert_bytes) < 130 or len(der_cert_bytes) > 65535:
        return base64.b64encode(der_cert_bytes).decode('utf-8')# cert too short or too long, skip stripping
    asnlen = der_cert_bytes[1]
    if asnlen >= 128:
        lenlen = asnlen-128
    else:
        lenlen = 0
    index = index + lenlen + 2
    # start of tbscert length
    index = nextasnindex(index, asn1_bytes=der_cert_bytes)
    # start of signatureAlgorithem length:
    # we don't change sigAlgo because it's need to be same with root's public key algo 
    # to make Openssl consider it self-signed
    index = nextasnindex(index, asn1_bytes=der_cert_bytes)
    # start of signature length: we change it to zero-length bitstring
    der_cert_bytes[index:index+2] = [1, 0]
    index += 2 #now index marks end of new cert file
    der_cert_bytes = der_cert_bytes[0:index]
    der_cert_bytes[2] = (index-4)//256 #new certificate asn1 size, minus asn1 header, 
    der_cert_bytes[3] = (index-4)%256 #assume cert size between 128B ~ 65kB
    encoded = base64.b64encode(der_cert_bytes).decode('utf-8')
    return encoded


filepath = sys.argv[1]
infile = open(filepath, "r")
der_certs = decode_pems_to_binary(infile.read())
infile.close()

outfile = open(filepath, "w")

for cert in der_certs:
    outfile.write("-----BEGIN CERTIFICATE-----\n")
    outfile.write("\n".join(textwrap.wrap(minifycert(cert), 64)))
    outfile.write("\n-----END CERTIFICATE-----\n")
outfile.close()