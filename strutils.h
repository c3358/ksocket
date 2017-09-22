#ifndef _COMM_STRUTILS_H_
#define _COMM_STRUTILS_H_

static inline void S_strcpyn(char *dst, const char *src, size_t n)
{
    size_t c = 0;

    while(c != (n - 1) && src[c] != '\0')
    {
        dst[c] = src[c];
        c++;
    }

    dst[c] = '\0';
}

static inline char *strupr(char *s)
{
	char *p = s;

	while(*p != '\0')
	{
		if(islower(*p))
		{
			*p = toupper(*p);
		}
		p++;
	}
	return s;
}

static inline void byte2hex(unsigned char c, char s[2])
{
	int low,high;
	static char slots[] = "0123456789ABCDEF";

	s[0] = 0;
	s[1] = 0;


	low = c % 16;
	high = (c / 16) % 16;

	s[0] = slots[low];
	s[1] = slots[high];
}


static inline void bin2hex(char *out, int outlen, unsigned char *buf, int len)
{
	char tmp[2];
	int i;
	int pos;
	int s_len = outlen - 1;

	bzero(out, outlen);

	pos = 0;

	for(i = 0; i < len; i++)
	{
		byte2hex(buf[i], tmp);

		if(s_len - pos >= 2)
		{
			out[pos] = tmp[1];
			out[pos + 1] = tmp[0];
			pos += 2;
		}
	}
}

static inline bool safe_strchk(const char *src, size_t length)
{
	size_t i;

	for(i = 0 ; i < length; i++)
	{
		if(src[i] == '\0')
			return true;
	}

	return false;
}


//#define safe_strcpy(dst, src) S_strcpyn(dst, src, sizeof(dst))
#define s_strcpy(dst, src) S_strcpyn(dst, src, sizeof(dst))

#define strrange(s) s[sizeof(s) - 1] = '\0';

#endif