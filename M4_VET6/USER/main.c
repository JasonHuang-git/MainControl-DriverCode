#include <bsp.h>

int main(void)
{
    uint8_t key_code = 0;

    bsp_init();
//    udp_test_app();
    
	tcp_client_test_app();  	//TCP Serverģʽ
	while(1)
	{
		key_code = key_scan(0);
		if(key_code == KEY0_PRES)		//��KEY1����������
		{
			if((tcp_client_flag & 1<<5)) printf("TCP�ͻ��������Ѿ�����,�����ظ�����\r\n");	//������ӳɹ�,�����κδ���
			else tcp_client_test_app();		//���Ͽ����Ӻ�,����tcp_server_test()����
		}
		delay_ms(10);
	}
//    while(1)
//    {
//        key_code = key_scan(0);
//        if(key_code == KEY0_PRES)               // KEY0������
//        {
//            // ������ӳɹ�,�����κδ���
//            if((udp_demo_flag & 1<<5)) 
//                printf("UDP�����Ѿ�����,�����ظ�����\r\n");	
//			else udp_test_app();		
//        }

//    }
}
