#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
	#pragma warning(disable:4996)
#endif

#define MAX_LEN         14

uint32_t g_count = 0;

uint32_t obfuscateWord(const unsigned char *e, int len)
{
	uint32_t t = 0x1509c000;

	for (int i = 0; i < len; i++)
	{
		t = (t*33) ^ e[i];
	}
	return t;
}

uint32_t earlyExit(uint32_t hash, uint32_t testHash, uint32_t pow33)
{
	uint32_t hi = (hash | 0x7f) * pow33 + pow33 - 1;
	uint32_t lo = (hash & ~0x7f) * pow33;

	if ((lo < hi && (lo > testHash || hi < testHash)) ||
	    (lo > hi && (lo > testHash && hi < testHash)))
	{
		return 1;
	}
	return 0;
}

void earlyExitBruteForce(uint32_t hash, int len)
{
	unsigned char word[MAX_LEN + 1] = {0};

	// Init word
	int i;
	for (i = 0; i < len; i++)
	{
		word[i] = 'a';
	}

	// Brute force
	do
	{
		uint32_t hash0 = 33 * obfuscateWord(word, len - 5);
		if (earlyExit(hash0, hash, 33*33*33*33) == 0)
		{
			for (unsigned char c0 = 'a'; c0 <= 'z'; c0++)
			{
				if (earlyExit(33 * (hash0 ^ c0), hash, 33*33*33) == 0)
				{
					uint32_t hash1 = 33 * (hash0 ^ c0);
					for (unsigned char c1 = 'a'; c1 <= 'z'; c1++)
					{
						if (earlyExit(33 * (hash1 ^ c1), hash, 33*33) == 0)
						{
							uint32_t hash2 = 33 * (hash1 ^ c1);
							for (unsigned char c2 = 'a'; c2 <= 'z'; c2++)
							{
								if (earlyExit(33 * (hash2 ^ c2), hash, 33) == 0)
								{
									uint32_t hash3 = 33 * (hash2 ^ c2);
									for (unsigned char c3 = 'a'; c3 <= 'z'; c3++)
									{
										uint32_t c4 = (33 * (hash3 ^ c3)) ^ hash;
										if (c4 >= 'a' && c4 <= 'z')
										{
											word[len - 5] = c0;
											word[len - 4] = c1;
											word[len - 3] = c2;
											word[len - 2] = c3;
											word[len - 1] = (unsigned char) c4;
											printf("%s\n", word);
											g_count++;
										}
									}
								}
							}
						}
					}
				}
			}
		}

		// Next
		for (i = 0; i < len - 5; i++)
		{
			word[i]++;
			if (word[i] <= 'z')
			{
				break;
			}
			word[i] = 'a';
		}
		if (i >= len - 6)
		{
			fprintf(stderr, "*");
		}
	} while (i < len - 5);
}

int main(int argc, char *argv[])
{
	uint32_t hash;
	uint32_t len;

	if (argc == 2)
	{
		printf("\"%s\": %u", argv[1], obfuscateWord((unsigned char*) argv[1], strlen(argv[1])));
		return 0;
	}
	if (argc != 3 ||
		sscanf(argv[1], "%u", &hash) != 1 ||
		sscanf(argv[2], "%u", &len)  != 1)
	{
		fprintf(stderr, "Usage is:\n%s target-hash length\n%s string-to-hash\n", argv[0], argv[0]);
		return 1;
	}
	if (len < 5 || len > MAX_LEN)
	{
		fprintf(stderr, "Bad length (5 to %u): %u\n", MAX_LEN, len);
		return 1;
	}

	fprintf(stderr, "Early exit brute forcing...\n|--------------------------|\n|");
	earlyExitBruteForce(hash, len);
	fprintf(stderr, "|\nDone\nFound %u\n", g_count);

	return 0;
}
