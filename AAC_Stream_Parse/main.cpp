#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct AAC_ADTS_Header
{
    uint8_t profile;
    uint8_t sampling_frequency_index;
    uint8_t channel_configuration;
    uint16_t aac_frame_length;
}AAC_ADTS_Header;

int main()
{
    FILE *in_fp = fopen("test.aac", "rb");
    if (!in_fp)
    {
        printf("%s\n", "open file error");
        return -1;
    }

    char header[7];
    char aac_es[4096];
    size_t nread = 0;
    AAC_ADTS_Header aac_adts_header;
    while (!feof(in_fp))
    {
        memset(header, 0, sizeof(header));
        memset(aac_es, 0, sizeof(aac_es));
        memset(&aac_adts_header, 0, sizeof(aac_adts_header));
        nread = fread(header, 1, sizeof(header), in_fp);
        if (nread != sizeof(header))
        {
            if (feof(in_fp))
            {
                fclose(in_fp);
                return 0;
            }
            printf("%s\n", "read header failed");
        }

        aac_adts_header.profile = ((header[2] & 0xc0) >> 6);
        aac_adts_header.sampling_frequency_index = ((header[2] & 0x3c) >> 2);
        aac_adts_header.channel_configuration = (((header[2] & 0x01) << 2) | ((header[3] & 0xc0) >> 6));
        aac_adts_header.aac_frame_length = (header[3] & 0x03);
        aac_adts_header.aac_frame_length <<= 8;
        aac_adts_header.aac_frame_length |= header[4];
        aac_adts_header.aac_frame_length <<= 3;
        aac_adts_header.aac_frame_length |= ((header[5] & 0xe0) >> 5);

        nread = fread(aac_es, 1, aac_adts_header.aac_frame_length - sizeof(header), in_fp);
        if (nread != aac_adts_header.aac_frame_length - sizeof(header))
        {
            printf("%s\n", "read aac audio stream failed");
            return -1;
        }

        printf("profile: %d, sampling_frequency_index: %d, channel_configuration: %d, aac_frame_length: %d\n", aac_adts_header.profile,
        aac_adts_header.sampling_frequency_index, aac_adts_header.channel_configuration, aac_adts_header.aac_frame_length);
    }

    fclose(in_fp);
    return 0;
}