#include <wincompat.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <termios.h>

static struct termios s_currentTermios;

int tcgetattr(fd_t fildes, struct termios* termios_p)
{
//	if (GetFileType((HANDLE)_get_osfhandle(fildes))== FILE_TYPE_PIPE)
	{
		memset(termios_p, 0, sizeof(*termios_p));
		termios_p->c_iflag = IGNBRK;
		termios_p->c_oflag = ONOCR | ONLRET;
		termios_p->c_lflag = ICANON | ISIG | ECHO | ECHOE | ECHOK;
		return 0;
	}
	return -1;
}
int tcsetattr(fd_t fd, int optional_actions, const struct termios* termios_p)
{
	UNREFERENCED_PARAMETER(optional_actions);
//	if (GetFileType((HANDLE)_get_osfhandle(fd))== FILE_TYPE_PIPE)
	{
		memcpy(&s_currentTermios, termios_p, sizeof(s_currentTermios));
		return 0;
	}
	return -1;
}
int	tcflush(fd_t fd, int q)
{
	UNREFERENCED_PARAMETER(fd);
	UNREFERENCED_PARAMETER(q);
	return -1;
}
speed_t	cfgetispeed(const struct termios* p)
{
	UNREFERENCED_PARAMETER(p);
	return 115200;
}
speed_t	cfgetospeed(const struct termios* p)
{
	UNREFERENCED_PARAMETER(p);
	return 115200;
}
int	cfsetispeed(struct termios*p , speed_t s)
{
	UNREFERENCED_PARAMETER(p);
	UNREFERENCED_PARAMETER(s);
	return -1;
}
int	cfsetospeed(struct termios*p , speed_t s)
{
	UNREFERENCED_PARAMETER(p);
	UNREFERENCED_PARAMETER(s);
	return -1;
}
void cfmakeraw(struct termios* p)
{
	UNREFERENCED_PARAMETER(p);
}
void win_init()
{
//	setvbuf(stdout, NULL, _IONBF, 0);
	DWORD consoleMode =0;
	HANDLE hConsole = { GetStdHandle(STD_OUTPUT_HANDLE) };
	GetConsoleMode(hConsole, &consoleMode);
//	SetConsoleMode(hConsole, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	_setmode(_fileno(stdin), _O_BINARY);
}
