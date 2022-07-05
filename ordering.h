#ifndef ordering_H
#define ordering_H
enum  class ordering
{
	none,
	name, //print name
	phone, //print phone
	group, //print group

};
enum class codes_client_server: char
{
    SUCCESS = 0,
    OUTPUT_END,
    TROUBLE,
    QUIT_OR_STOP,
};
#endif // ordering_H
