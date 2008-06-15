
#if 1
#define DEBUG_REG printk
#else
#define DEBUG_REG 
#endif

void ReadDataFromRegister(robo_info_t *robo, uint16 page_num,uint16 addr_num, uint16 len, void* data)
{

	uint32 val32 = 0,val64[2];
	uint16 val16 =0,val48[3];//,val32[2],val64[4];
	memset(val48,0,6);
	memset(val64,0,8);
	//memset(val32,0,4);
	DEBUG_REG("Read ioctl Page[0x%02x] Addr[0x%02x] len[%d].\n", page_num, addr_num, len);
	switch (len)
	{
		case 1:
		case 2:
		{
			robo->ops->read_reg(robo, page_num, addr_num, &val16, len);
			DEBUG_REG("IRead 8/16 bit Page[0x%02x]addr[0x%02x]len[%d]val[0x%04x].\n",
					page_num, addr_num, len, val16);
			memcpy(data,&val16,2);
			break;
      		}
		case 4:
		{
			robo->ops->read_reg(robo, page_num, addr_num, &val32, len);
			DEBUG_REG("IRead 32bit Page[0x%02x]addr[0x%02x]len[%d]val[0x%08x].\n",
					page_num, addr_num, len, val32);
			memcpy(data,&val32,4);
			break;
		}
		case 6:
		{
			robo->ops->read_reg(robo, page_num,addr_num, &val48, len);
			DEBUG_REG("IRead 48bit Page[0x%02x]addr[0x%02x]len[%d]val[0x%04x-0x%04x-0x%04x].\n",
				page_num, addr_num, len, val48[0], val48[1], val48[2]);
			memcpy(data,&val48,6);
			break;
		}
		case 8:
		{
			robo->ops->read_reg(robo, page_num, addr_num, &val64, len);
			DEBUG_REG("IRead 64bit Page[0x%02x]addr[0x%02x]len[%d]val[0x%08x-0x%08x].\n",
				page_num, addr_num, len, val64[0], val64[1]);
			memcpy(data,&val64,8);
			break;
		}
	}
}



void WriteDataToRegister(robo_info_t *robo,uint16 page_num,uint16 addr_num, uint16 len, void* data)
{
        DEBUG_REG("Write ioctl Page[0x%02x]Addr[0x%02x]len[%d].\n",page_num,addr_num,len);
        switch (len)
        {
                case 1:
                case 2:
                {
        		DEBUG_REG("Write 2byte Page[0x%02x]addr[0x%02x]len[%d]val[0x%04x].\n",
					page_num, addr_num, len, *((uint16 *)data));
        		robo->ops->write_reg(robo, page_num, addr_num, data, len);
       			if (page_num < 0x10 || page_num > 0x17) {
        			robo->ops->read_reg(robo, page_num, addr_num, data, len);
	                	DEBUG_REG("Reload Page[0x%02x]addr[0x%02x]len[%d]val[0x%04x].\n",
					page_num, addr_num, len, *((uint16 *)data));
			}
                        break;
                }
                case 4:
                {
                        DEBUG_REG("Write 4byte Page[0x%02x]addr[0x%02x]len[%d]val[0x%08x].\n",
                                        page_num, addr_num, len, *((uint32 *)data));
                        robo->ops->write_reg(robo, page_num, addr_num, data, len);
       			if (page_num < 0x10 || page_num > 0x17) {
 	                       robo->ops->read_reg(robo, page_num, addr_num, data, len);
        	                DEBUG_REG("Reload Page[0x%02x]addr[0x%02x]len[%d]val[0x%08x].\n",
                                        page_num, addr_num, len, *((uint32 *)data));
			}
                        break;
                }
                case 6:
                {
                        DEBUG_REG("Write 6byte Page[0x%02x]addr[0x%02x]len[%d]val[0x%04x-0x%04x-0x%04x].\n",
                                page_num, addr_num, len, *((uint16 *)data),*((((uint16 *)data)+1)),
				*(((uint16 *)data)+2));
		        robo->ops->write_reg(robo, page_num, addr_num, data, len);
                        robo->ops->read_reg(robo, page_num, addr_num, data, len);
                        DEBUG_REG("Reload Page[0x%02x]addr[0x%02x]len[%d]val[0x%04x-0x%04x-0x%04x].\n",
				page_num, addr_num, len,*((uint16 *)data),*((((uint16 *)data)+1)),
                                *(((uint16 *)data)+2));
                        break;
                }
                case 8:
                {
                        DEBUG_REG("Write 8byte Page[0x%02x]addr[0x%02x]len[%d]val[0x%08x-0x%08x].\n",
                                page_num, addr_num, len, *((uint32*)data),*(((uint32 *)data)+1));
                        robo->ops->write_reg(robo, page_num, addr_num, data, len);
                        robo->ops->read_reg(robo, page_num, addr_num, data, len);
                        DEBUG_REG("Reload Page[0x%x]addr[0x%x]len[%d]val[0x%08x-0x%08x].\n",
                                page_num, addr_num, len,*((uint32 *)data), *(((uint32 *)data)+1));
                        break;
                }
        }
}

