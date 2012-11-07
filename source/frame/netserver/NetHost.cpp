#include "frame/netserver/NetHost.h"
#include "frame/netserver/NetConnect.h"
#include "mdk/Socket.h"
#include "mdk/atom.h"
using namespace std;

namespace mdk
{

NetHost::NetHost()
:m_pConnect(NULL)
{
}

NetHost::NetHost(const NetHost& obj)
{
	AtomAdd(&obj.m_pConnect->m_useCount, 1);
	m_pConnect = obj.m_pConnect;
}

NetHost& NetHost::operator=(const NetHost& obj)
{
	if ( m_pConnect == obj.m_pConnect ) return *this;
	if ( NULL != m_pConnect ) m_pConnect->Release();
	AtomAdd(&obj.m_pConnect->m_useCount, 1);
	m_pConnect = obj.m_pConnect;
	return *this;
}

NetHost::~NetHost()
{
	if ( NULL != m_pConnect ) m_pConnect->Release();
}
 
bool NetHost::Send(const unsigned char* pMsg, unsigned short uLength)
{
	return m_pConnect->SendData(pMsg, uLength);
	return true;
}

uint32 NetHost::GetLength()
{
	return m_pConnect->GetLength();
}

bool NetHost::Recv( unsigned char* pMsg, unsigned short uLength, bool bClearCache )
{
	return m_pConnect->ReadData( pMsg, uLength, bClearCache );
}

void NetHost::Close()
{
	m_pConnect->Close();
}

bool NetHost::IsServer()
{
	return m_pConnect->IsServer();
}

int NetHost::ID()
{
	return m_pConnect->GetID();
}

//����ĳ����
void NetHost::InGroup( int groupID )
{
	m_pConnect->InGroup(groupID);
}

//��ĳ����ɾ��
void NetHost::OutGroup( int groupID )
{
	m_pConnect->OutGroup(groupID);
}

}  // namespace mdk
