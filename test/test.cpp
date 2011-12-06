#include "test.protobuf.inc"

#include "SsuObject.h"
namespace ssutest
{
#include "test.ssu.cpp"
}

template<typename T>
void testIt(T& st)
{
	st.setSa(true);
	st.setSb(-1);
	T::StructA * sa = st.addSc();
	sa->setAbc(15555555);
	T::StructA * sa2 = st.mutableSd();
	sa2->setAbc(3838555);
	st.addSe(105);
	st.addSe(-999);
	st.setSf(T::eC);
	st.addSg(T::eB);
	st.addSg(T::eA);
	st.setSi("abcde");
	st.addSj("fghi");
	st.addSj("jklm");
	st.setSk(1000000.0f);
	st.addSl(-1000.0);
	st.addSl(1000.0);
}

#include <windows.h>
int main(int, char * argv[])
{
// 	DWORD ts;
// 	ts = GetTickCount();
// 	for(int i = 0; i < 1000000; ++ i)
	{
		StructTest st;
		testIt(st);
		std::string n;
		st.AppendToString(&n);
		for(auto it = n.begin(); it != n.end(); ++ it)
		{
			printf(" %02X", (unsigned char)*it);
		}
		printf("\n");
	}
// 	printf("elapsed: %ums\n", GetTickCount() - ts);
// 	ts = GetTickCount();
// 	for(int i = 0; i < 1000000; ++ i)
	{
		ssutest::StructTest st2;
		testIt(st2);
		std::vector<unsigned char> buf;
		st2.pack(buf);
		for(auto it = buf.begin(); it != buf.end(); ++ it)
		{
			printf(" %02X", (unsigned char)*it);
		}
		printf("\n");

		ssutest::StructTest st3;
		st3.unpack(&buf[0], buf.size());
		printf("end\n");
	}
// 	printf("elapsed: %ums\n", GetTickCount() - ts);
	return 0;
}
