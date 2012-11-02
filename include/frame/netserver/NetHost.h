#ifndef MDK_NETHOST_H
#define MDK_NETHOST_H

#include "mdk/FixLengthInt.h"
#include "mdk/Lock.h"
#include <map>

namespace mdk
{
	
class NetConnect;
class Socket;
/**
	����������
	��ʾһ������������������˽�й��캯����ֻ�������洴�����û�ʹ��
*/
class NetHost
{
	friend class NetConnect;
public:
	~NetHost();
	/*
		����Ψһ��ʶ
		��ʵ�ʾ�������������ӵ�SOCKET�����������ֱ��ʹ��socket���api��������socket��io��close��
		��Ϊcloseʱ���ײ���Ҫ�������������ֱ��ʹ��socketclose()����ײ����û����ִ��������,������Ӳ�����
		io�����ײ��Ѿ�ʹ��io�������ֱ��ʹ��api io������io��������һ���ײ�io���������������ݴ���
	*/
		
	int ID();
	//ȡ���Ѿ��������ݳ���
	uint32 GetLength();
	/*
		�ӽ��ջ����ж�����
			������
				pMsg		��Ϣ������յ�����
				uLength		��Ҫ���յĳ���
				bClearCache	�Ƿ񽫽��յ������ݴӻ����������trueɾ����false����
				���籨�ĸ�ʽ��2byte���ݳ���+��������
				OnMsg()�ڽ����߼�����
					1.Recv(msg, 2, true);
					2.����msg�õ����ݳ��ȣ�����Ϊ256
					3.Recv(msg, 256, true)
					��
					���3.���ﷵ��false����ʾʵ�ʵ�������<256��������ȡ
					
					��ʱ���û���2��ѡ��
					ѡ��1��
					ѭ��ִ��Recv ֱ���ɹ��������sleep��CPUֱ��100%�����sleep����ӦЧ�ʽ���
					ѡ��2��
					��256����������ֱ��return�˳�OnMsg���´�OnMsg����ʱ���ٳ���Recv
					�ŵ㣬û��sleep,����CPU
					ȱ�㣺�û�����������֯���û���ҪΪ��������ά��һ��int����������ճ��ȣ�
					Ҳ�����û���Ҫ�Լ�ά��һ���б������ӶϿ�ʱ��Ҫ���б�ɾ������������
					
					����false��bClearCache��������������
					1.Recv(msg, 2, false);
					2.����msg�õ����ݳ��ȣ�����Ϊ256
					3.Recv(msg, 256+2, true)//�������ĳ�����256+2
						���Recv�ɹ���ֱ�Ӵ���
						���Recvʧ�ܣ���ʾ�������ݲ�������Ϊ1.���ﴫ����false��
						���ĳ�����Ϣ����ӽ��ջ�����ɾ�������ԣ��û�����ֱ��return�˳�OnMsg,
						�´�OnMsg����ʱ�������Դ������϶����������ݳ�����Ϣ
			
			����ֵ��
			���ݲ�����ֱ�ӷ���false
			��������ģʽ�������Ѿ����û��������Ϣ�ȴ�����Ϣ����ʱ����OnMsg������
	*/
	bool Recv(unsigned char* pMsg, unsigned short uLength, bool bClearCache = true );
	/**
		��������
		����ֵ��
			��������Чʱ������false
	*/
	bool Send(const unsigned char* pMsg, unsigned short uLength);
	void Close();//�ر�����
	/**
		���ֶ���ʹ���꣬����Free()
		�û���OnConnect OnMsg OnCloseʱ���л���õ���������ָ��
		�˳����Ϸ�����ָ��Ϳ��ܱ������ͷš�
		
		  ���ͻ��п�����Ҫ�����ָ�룬���Լ���ҵ��ʹ�ã���ôÿ�α����ָ�룬������Holdһ�Σ�������֪����һ���߳��ڷ��ʸ�ָ�룬ÿ�������Freeһ�Σ��൱������ָ�����ü����ĸ���
		  
			��ʹ�ð���
			����1
			OnConnect(NetHost *pClient)
			{
			pClient->hold()
			pClient->send()
			pClient->free()
			�ӿ�˵��˵�ˣ�pClient�ڻص��˳�ǰ�����԰�ȫ���ײ㲻���ͷţ�hold free������������󣬵�����Ҫ 
			}
			
			  ����2
			  OnConnect(NetHost *pClient)
			  {
			  ���ӽ���������ָ�뵽map�У���ҵ��������Ҫ�������������Ϣ��ת����ĳ��ҹ�������ҵ������ʱ��ʹ��
			  pClient->hold()
			  Lock map
			  Map.insert(pClient->ID(),pClient);//����pClient��������1�Σ�hold��1��
			  unlock map
			  }
			  OnCloseConnect (NetHost *pClient)
			  {
			  ���ӶϿ���ָ��û�����ˣ���mapɾ������free
			  Lock map
			  pClient->free()
			  map.del(pClient->ID()) // pClient����mapɾ���� 1��hold�ˣ�free1��
			  unlock map
			  }
			  ������ǰ����
			  ����ʽ1
			  OnMsg(NetHost *pClient)
			  {
			  ��������id Ϊ1�����
			  Lock map
			  NetHost *otherHost = map.find(1)//�����2�α��棬û��hold�� ����1��
			  Unlock
			  ʹ��otherHost��֪ͨ����ܵ�����//�����ʱ���1�����˳���Ϸ���������߳��л�����OnCloseConnect��ָ�뱻free
			  2�����
			  1.	�ײ㶨ʱ�����ָ��״̬��ʱ��δ���ָ�벻�ᱻ�ͷţ��߳��л���OnMsg������֪ͨ�����������ѶϿ�����ȫ�˳�onMsg 
			  2.	�ײ㶨ʱ�����õ�����ʱ��㣬����ָ��ķ��ʼ�����0����ɾ��ָ�롣otherHost��ΪҰָ�룬�߳��л���OnMsg�������ܵ�����֪ͨʱ������Ұָ�룬ϵͳ����
			  }
			  ����ʽ2
			  OnMsg(NetHost *pClient)
			  {
			  ��������id Ϊ1�����
			  Lock map
			  NetHost *otherHost = map.find(1)//�����2�α��棬û��hold�� ����1��
			  Unlock
			  otherHost->hold()�������ͬ�ϣ�������holdʱ���Ѿ���Ұָ��
			  ʹ��otherHost��֪ͨ����ܵ�����
			  otherHost->free()
			  
				}
				
				  ��ȷ��ʽ1
				  OnMsg(NetHost *pClient)
				  {
				  ��������id Ϊ1�����
				  Lock map
				  NetHost *otherHost = map.find(1)//�����2�α��棬û��hold�� ����1��
				  ʹ��otherHost��֪ͨ����ܵ���������ʹ�߳��л���free�̣߳���Ϊlock��free�̻߳ᱻ���𣬵ȴ�����unlock���ײ㲻���ͷţ���ȫ
				  Unlock
				  �˳�OnMsg
				  }
				  
					��ȷ��ʽ2
					OnMsg(NetHost *pClient)
					{
					��������id Ϊ1�����
					Lock map
					NetHost *otherHost = map.find(1)//�����2�α��棬û��hold�� ����1��
					otherHost->hold()��ͬ����unlock֮ǰ��free�̲߳�����ִ�У��ײ㲻�����ͷţ�
					Unlock
					ʹ��otherHost��֪ͨ����ܵ��������Ѿ�hold2�Σ���ʹ�߳��л���free�̣߳����ʼ������Ǵ���1���ײ㲻���ͷţ���ȫ
					otherHost->free()//ʹ����ϣ��ͷŷ��ʣ��˳�OnMsg
					
					  }
		*/
	void Hold();
	/**
		�ͷŶ���
		����ȫ�����ʸ�ָ���Free()����������Hold()��ͬ������ײ���Զ���ͷŸ�ָ�롣
	*/
	void Free();
	/*
		����������һ������
		NetServer����Connect���ӵ�һ��������ʱ��������NetHost��ָ��һ��������	
	*/
	bool IsServer();
	
	//////////////////////////////////////////////////////////////////////////
	//ҵ��ӿڣ���NetEngine::BroadcastMsgż��
	void InGroup( int groupID );//����ĳ���飬ͬһ�������ɶ�ε��ø÷���������������
	void OutGroup( int groupID );//��ĳ����ɾ��
	
private://˽�й��죬����ֻ��NetConnect�ڲ����������ͻ�ֻ��ʹ��
	NetHost( bool bIsServer );

private:
	NetConnect* m_pConnect;//���Ӷ���ָ��,����NetConnect��ҵ���ӿڣ�����NetConnect��ͨ�Ų�ӿ�
	bool m_bIsServer;//�������ͷ�����
	std::map<int,int> m_groups;//��������
	Mutex m_groupMutex;//m_groups�̰߳�ȫ
	
};

}  // namespace mdk
#endif//MDK_NETHOST_H
