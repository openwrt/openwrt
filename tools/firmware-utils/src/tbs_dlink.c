
#include <getopt.h>             /* for getopt() */
#include "tbs_dlink.h"

int ConvertEndian(int val,int endian)
{
  int i;
  unsigned char *p;
  unsigned char *q;

  i = val;
  p = ( unsigned char * ) &val;
  q = ( unsigned char * ) &i;

  if(endian == BIGENDIAN)
  {
    *q = *(p+3);
    *(q+1) = *(p+2);
    *(q+2) = *(p+1);
    *(q+3) = *p;
  }

  return i;
}

int tbs_crc_file(FILE *fp , unsigned int offset , unsigned long *checksum)
{
  unsigned char buf[BUFLEN];
  unsigned long crc = 0;
  unsigned int length = 0;
  unsigned int bytes_read;
  
  fseek(fp, offset , SEEK_SET);

  while((bytes_read = fread(buf, 1, BUFLEN, fp)) > 0)
  {
    unsigned char *cp = buf;

    if(length + bytes_read < length)
      return 0;

    length += bytes_read;
    while(bytes_read--)
      crc =(crc << 8) ^ crctab[((crc >> 24) ^ *cp++) & 0xFF];
  }
 
  if(ferror(fp))
    return 0;

  for(; length; length >>= 8)
    crc =(crc << 8) ^ crctab[((crc >> 24) ^ length) & 0xFF];

  crc = ~crc & 0xFFFFFFFF;
  *checksum = crc;

  return 1;
}

int CreateImgFile(const unsigned char *outimg, unsigned long ksize, unsigned long rsize, int board_endian, const unsigned char *region, 
	const unsigned char *model, const unsigned char *product)
{
  char *img_orig;
  update_hdr_t  image_header;

  FILE    *pfin;
  FILE    *pfout;

  int     iImgFileLength = 0;      /* count how many bytes have been write to IMG file */
  int     iReadCount;
  int     iWriteCount;
  int     tmp;
  int     i;
  unsigned long checksum_result;
    
  memset(&image_header,0,sizeof(update_hdr_t));

  strcpy( image_header.product, product);
  strcpy( image_header.version, TBS_PRODVERSION);
  strcpy( image_header.img_type, TBS_IMGTYPE);
  strcpy( image_header.board_id, TBS_BOARDID);

  //for netgear
  strcpy( image_header.region, region);
  strcpy( image_header.model_name,  model);
  strcpy( image_header.swversion,  TBS_SWVERSION);

  image_header.kernel_size = ksize;
  image_header.rootfs_size = rsize;

  img_orig = (unsigned char *)malloc(image_header.kernel_size+image_header.rootfs_size);
  memset(img_orig, 0xffff, image_header.kernel_size+image_header.rootfs_size);

  pfin = fopen(outimg,"rb");
  if(pfin == NULL)
  {
    printf("Can't open image file: %s\n",outimg);
    return 1;
  }

  iWriteCount=0;
  while(!feof(pfin)) {
    iReadCount = fread(img_orig+iWriteCount,1,4096,pfin);
    iWriteCount += iReadCount;
  }

  fclose(pfin);

  pfout = fopen(outimg,"wb+");
  if(pfout == NULL)
  {
    printf("Can't open output file: %s\n",outimg);
    return 1;
  } 

  for(i=0; i<sizeof(update_hdr_t); i++)
  {
    fputc(0xffff,pfout);
    iImgFileLength++;
  }

  image_header.kernel_offset = iImgFileLength;
  image_header.rootfs_offset = image_header.kernel_size + image_header.kernel_offset;
  image_header.image_len = image_header.kernel_size+image_header.rootfs_size;

  fwrite(img_orig, 1, image_header.kernel_size+image_header.rootfs_size, pfout);
  free(img_orig);

/******************************************************************
        Deal with image header            
******************************************************************/

  image_header.rootfs_offset = ConvertEndian(image_header.rootfs_offset, board_endian);
  image_header.rootfs_size = ConvertEndian(image_header.rootfs_size, board_endian);
  image_header.kernel_offset = ConvertEndian(image_header.kernel_offset, board_endian);
  image_header.kernel_size = ConvertEndian(image_header.kernel_size, board_endian);
  image_header.image_len = ConvertEndian(image_header.image_len, board_endian);

  if( tbs_crc_file(pfout , sizeof(update_hdr_t) , &checksum_result ) )
  {
    image_header.image_checksum = ConvertEndian( checksum_result , board_endian);

    if(fseek(pfout,0,SEEK_SET) == -1)
    {
          printf("Fail to point image header.\n");
          fclose(pfout);
          remove(outimg);
          return 1;                         /* fail to lseek */
    }

    iWriteCount = fwrite(&image_header,sizeof(update_hdr_t),1,pfout);
    if(iWriteCount != 1)  /* fail to write ? */
    {
         printf("Fail to write checksum to IMG file.\n");
         fclose(pfout);
         remove(outimg);
         return 1;
    }
  }


/******************************************************************
        set image.img file crc           
******************************************************************/
  
  tbs_crc_file( pfout , 0 , &checksum_result );
  checksum_result = ConvertEndian( checksum_result, board_endian);

  if(fseek(pfout,0,SEEK_END) == -1)
  {
    printf("Fail to point IMG file tail.\n");
    fclose(pfout);
    remove(outimg);
    return 1;                         /* fail to lseek */
  }

  if(fwrite( &checksum_result , 1, 4 , pfout )  <  4 )
  {
    printf("Fail to write file_checksum to IMG file.\n");
    fclose(pfout);
    remove(outimg);
    return 1;
  }
  
  fclose(pfout);
  
  return 0;
}

int main(int argc, char *argv[])
{
  int ret = 0;
  int bend = LITTLEENDIAN;
  char c;
  unsigned long ksize = 0x1D0000;
  unsigned long rsize = 0x45E000;
  unsigned char region[REGION_LEN] = TBS_REGION;
  unsigned char model[MODEL_LEN] = TBS_MODEL_NAME; 
  unsigned char product[PRODUCT_NAME_LEN] = TBS_PRODUCT;

  while ((c = getopt(argc, argv, "br:k:g:m:p:")) != -1) {
    switch (c) {
      case 'b':
        bend = BIGENDIAN;
        break;
      case 'r':
        sscanf(optarg, "0x%x", &rsize);
        break;
      case 'k':
        sscanf(optarg, "0x%x", &ksize);
        break;
      case 'm':
        strcpy(model, optarg);
        break;
      case 'g':
        strcpy(region, optarg);
        break;
      case 'p':
        strcpy(product, optarg);
        break;
      default:
	printf("invalid option: -%c\n", optopt);
        break;
    }
  }
  
  if (optind == argc) {
     printf("no output file specified\n");
       exit(1);
  }

  return CreateImgFile(argv[optind], ksize, rsize, bend, region, model, product);
}
