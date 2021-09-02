#include <stdio.h>
#include <stdint.h>
#include <unordered_map>

using namespace std;

#ifdef _WIN32
	#pragma warning(disable:4996)
#endif

#define MAX_PREFIX_LEN  7
#define MAX_SUFFIX_LEN  7
#define MAX_LEN         (MAX_PREFIX_LEN + MAX_SUFFIX_LEN)

uint32_t g_count = 0;

struct prefixStruct
{
	unsigned char prefix[MAX_PREFIX_LEN + 1];
};

// Function stolen from (no clue what license) https://gist.github.com/moyix/f78e0b0d5724a1bf02e1a035e8bec136
uint32_t obfuscateWord(const unsigned char *e, int len)
{
	uint32_t t = 0x1509c000;

	for (int i = 0; i < len; i++)
	{
		t = (t*33) ^ e[i];
	}
	return t;
}

void fillPrefixes(unordered_multimap<uint32_t, prefixStruct> &hashes, int prefixLen)
{
	uint32_t     prefixIndex = 0;
	prefixStruct prefix = {0};

	// Init prefix
	int i;
	for (i = 0; i < prefixLen; i++)
	{
		prefix.prefix[i] = 'a';
	}

	// Fill prefixes
	do
	{
		// Insert
		hashes.insert({obfuscateWord(prefix.prefix, prefixLen), prefix});
		prefixIndex++;

		// Next
		for (i = 0; i < prefixLen; i++)
		{
			prefix.prefix[i]++;
			if (prefix.prefix[i] <= 'z')
			{
				break;
			}
			prefix.prefix[i] = 'a';
		}
	} while (i < prefixLen);
}

void reverseSuffixes(unordered_multimap<uint32_t, prefixStruct> &hashes, uint32_t hash, unsigned char suffix[MAX_SUFFIX_LEN + 1], uint32_t pos, uint32_t suffixLen)
{
	const uint32_t MODS[] = {
		0,   4,  8, 12, 16, 20, 24, 28, 32,  3,  7, 11, 15, 19, 23, 27, 
		31,  2,  6, 10, 14, 18, 22, 26, 30,  1,  5,  9, 13, 17, 21, 25, 29};
	const uint32_t DIVS[] = { // (i << 32 + 32) / 33
		0,           130150525,  260301049,  390451573,  520602097,  650752621,  780903145,  911053669, 
		1041204193, 1171354718, 1301505242, 1431655766, 1561806290, 1691956814, 1822107338, 1952257862, 
		2082408386, 2212558911, 2342709435, 2472859959, 2603010483, 2733161007, 2863311531, 2993462055, 
		3123612579, 3253763104, 3383913628, 3514064152, 3644214676, 3774365200, 3904515724, 4034666248, 4164816772};

	if (pos == 0)
	{
		auto range = hashes.equal_range(hash);
		for (auto it = range.first; it != range.second; ++it)
		{
			printf("%s%s\n", it->second.prefix, suffix);
			g_count++;
		}
	}
	else
	{
		pos--;
		uint32_t modHash    = (hash ^ 0x60) % 33;
		uint32_t lower5Hash = hash & 0x1f;
		for (int i = 0; i < 33; i++)
		{
			uint32_t modHash2 = modHash + MODS[i];
			if (modHash2 >= 33)
			{
				modHash2 -= 33;
			}
			if (modHash2 != 0)
			{
				// 0x61-0x7a
				// 011 00001 - 011 11010
				uint32_t lower5Char;

				if (lower5Hash >= modHash2)
				{
					lower5Char = lower5Hash ^ (lower5Hash - modHash2);
				}
				else
				{
					lower5Char = lower5Hash ^ (lower5Hash + 33 - modHash2);
				}
				if (lower5Char != 0 && lower5Char <= 0x1a)
				{
					uint32_t ch = 0x60 | lower5Char;
					suffix[pos] = (unsigned char) ch;
					reverseSuffixes(hashes, (hash ^ ch) / 33 + DIVS[i], suffix, pos, suffixLen);
				}
			}

			if (pos == suffixLen - 1)
			{
				fprintf(stderr, "*");
			}
		}
	}
}

int main(int argc, char *argv[])
{
	uint64_t prefixSize = 26;
	uint32_t hash;
	uint32_t prefixLen;
	uint32_t suffixLen;
	unsigned char suffix[MAX_SUFFIX_LEN + 1] = {0};

	if (argc == 2)
	{
		printf("\"%s\": %u", argv[1], obfuscateWord((unsigned char*) argv[1], strlen(argv[1])));
		return 0;
	}
	if (argc != 4 ||
		sscanf(argv[1], "%u", &hash)      != 1 ||
		sscanf(argv[2], "%u", &prefixLen) != 1 ||
		sscanf(argv[3], "%u", &suffixLen) != 1)
	{
		fprintf(stderr, "Usage is:\n%s target-hash prefix-length suffix-length\n%s string-to-hash\n", argv[0], argv[0]);
		return 1;
	}
	if (prefixLen < 1 || prefixLen > 7)
	{
		fprintf(stderr, "Bad prefix-length (1 to 7): %u\n", prefixLen);
		return 1;
	}
	if (suffixLen < 1 || suffixLen > 7)
	{
		fprintf(stderr, "Bad suffix-length (1 to 7): %u\n", suffixLen);
		return 1;
	}

	for (uint32_t i = 1; i < prefixLen; i++)
	{
		prefixSize *= 26;
	}
	prefixSize += prefixSize / 2;
	if (12 * prefixSize > SIZE_MAX)
	{
		fprintf(stderr, "Insufficient memory\n");
		return 1;
	}


	unordered_multimap<uint32_t, prefixStruct> hashes((size_t) prefixSize);

	fprintf(stderr, "Working on hash: %u\n", hash);

	fprintf(stderr, "Filling prefixes...");
	fillPrefixes(hashes, prefixLen);
	fprintf(stderr, " Done\nReversing suffixes...\n|---------------------------------|\n|");
	reverseSuffixes(hashes, hash, suffix, suffixLen, suffixLen);
	fprintf(stderr, "|\nDone\n");
	fprintf(stderr, "Found %u\n", g_count);

	return 0;
}
