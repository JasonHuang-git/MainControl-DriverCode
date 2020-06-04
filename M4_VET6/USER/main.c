#include <bsp.h>

int main(void)
{
    uint8_t key_code = 0;

    bsp_init();
//    udp_test_app();
    
	tcp_client_test_app();  	//TCP Server模式
	while(1)
	{
		key_code = key_scan(0);
		if(key_code == KEY0_PRES)		//按KEY1键建立连接
		{
			if((tcp_client_flag & 1<<5)) printf("TCP客户端连接已经建立,不能重复连接\r\n");	//如果连接成功,不做任何处理
			else tcp_client_test_app();		//当断开连接后,调用tcp_server_test()函数
		}
		delay_ms(10);
	}
//    while(1)
//    {
//        key_code = key_scan(0);
//        if(key_code == KEY0_PRES)               // KEY0按下了
//        {
//            // 如果连接成功,不做任何处理
//            if((udp_demo_flag & 1<<5)) 
//                printf("UDP连接已经建立,不能重复连接\r\n");	
//			else udp_test_app();		
//        }

//    }
}
