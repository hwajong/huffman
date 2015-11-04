#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

struct CharProb
{
	char ch;					// 문자                  
	float p;					// 확률          
};

// 이진트리 노드 
struct BTreeNode
{
	char ch;					// 문자
	float p;					// 확률 

	char lcode;
	char rcode;

	BTreeNode *left;			// left child
	BTreeNode *right;			// right child

	// 생성자 
	BTreeNode():left(NULL), right(NULL) {
}};

// 내림차순
static int compare_charprob_desc(const void *A, const void *B)
{
	const CharProb a = *(CharProb *) A;
	const CharProb b = *(CharProb *) B;

	if(a.p < b.p) return 1;
	else if(a.p > b.p) return -1;
	return 0;
}

// 허프만 코덱 클래스 
class HuffmanCodec 
{
private:
	int tsize;					// 테이블 사이즈
	CharProb *ptable;			// 확률 테이블 

	 map<char,string> code_map;	// 문자 - 코드 맵 

public:
	void encode(const char *text, const char *fname_out)
   	{
		//printf("* 각 문자별 발생빈도 분석\n");
		map < char, int >freqs;
		char ch;
		unsigned nchars = 0;
		const char* p = text;
		while (sscanf(p++, "%c", &ch) != EOF)
		{
			freqs[ch]++;		// 빈도 카운트 
			nchars++;
		}

		//printf("* 각 문자 별 빈도 테이블 생성\n");
		tsize = (int)freqs.size();
		ptable = new CharProb[tsize];

		int i = 0;
		map < char, int >::iterator iter;
		for(iter = freqs.begin(); iter != freqs.end(); ++iter, ++i)
		{
			ptable[i].ch = iter->first;
			ptable[i].p = iter->second / float (nchars);
		}

		// 내림차 순 소팅 
		qsort(ptable, tsize, sizeof(CharProb), compare_charprob_desc);

		// Encoding
		_huffman();

		// 출력파일 오픈 
		FILE *fp_out = fopen(fname_out, "w");
		assert(fp_out);

		printf("* 허프만 테이블 출력\n");
		printf("----------\n");
		printf("%i\n", tsize); 
		fprintf(fp_out, "%i\n", tsize);	// 첫 라인에 심볼테이블 사이즈 
		for(i = 0; i < tsize; i++)
		{
			// 탭으로 구분된 문자, 코드 출력
			printf("%c\t%s\n", ptable[i].ch, code_map[ptable[i].ch].c_str());
			fprintf(fp_out, "%c\t%s\n", ptable[i].ch, code_map[ptable[i].ch].c_str());
		}

		printf("* 인코딩된 비트스트링 출력\n");
		printf("----------\n");
		fprintf(fp_out, "\n");
		p = text;
		while (sscanf(p++, "%c", &ch) != EOF)
		{
			printf("%s", code_map[ch].c_str());
			fprintf(fp_out, "%s", code_map[ch].c_str());
		}
		printf("\n----------\n\n");

		//printf("* 자원해제\n");
		code_map.clear();
		delete[]ptable;
		fclose(fp_out);

		//printf("* 인코딩 완료\n");
	}

	void decode(const char *fname_in, const char *fname_out)
   	{
		// 입력파일 오픈
		FILE *fp_in = fopen(fname_in, "r");
		assert(fp_in);

		//printf("* 허프만 테이블 로딩\n");
		fscanf(fp_in, "%i", &tsize);	// 테이블 사이즈 
		char ch, code[128];
		fgetc(fp_in);			// skip newline
		for(int i = 0; i < tsize; i++)
		{
			ch = fgetc(fp_in);
			fscanf(fp_in, " %s", code);
			code_map[ch] = code;
			fgetc(fp_in);		// skip newline
		}
		fgetc(fp_in);			// skip newline

		// 출력파일 오픈  
		FILE *fp_out = fopen(fname_out, "w");
		assert(fp_out);

		printf("- 메세지 :  "); 
		string bits = "";
		while ((ch = fgetc(fp_in)) != EOF)
		{
			bits += ch;
			map < char, string >::iterator iter;
			for(iter = code_map.begin(); iter != code_map.end(); ++iter)
			{
				if(iter->second == bits)
				{
					bits = "";
					printf("%c", (*iter).first);
					fprintf(fp_out, "%c", (*iter).first);
				}
			}
		}
		printf("\n\n");

		//printf("* 자원해제\n");
		fclose(fp_out);
		fclose(fp_in);

		//printf("* 디코딩완료\n"); 
	}

private:
	void _huffman()
   	{
		//printf("* leaves 생성\n");
		vector < BTreeNode * >tops;	// top-nodes
		for(int i = 0; i < tsize; i++)
		{
			BTreeNode *n = new BTreeNode;
			n->ch = ptable[i].ch;
			n->p = ptable[i].p;
			tops.push_back(n);
		}

		//printf("* 이진트리 생성\n");
		int numtop = tsize;
		while (numtop > 1)
		{
			BTreeNode *n = new BTreeNode;

			// 빈도수가 가장 작은 2개를 빼서 처리 
			n->p = tops[numtop - 2]->p + tops[numtop - 1]->p; // 확률은 두 자식노드의 합
			n->left = tops[numtop - 2];
			n->right = tops[numtop - 1];

			// 코드 할당 
			if(n->left->p < n->right->p)
			{
				n->lcode = '0';
				n->rcode = '1';
			}
			else
			{
				n->lcode = '1';
				n->rcode = '0';
			}

			// 처리된 2개 제거 
			tops.pop_back();
			tops.pop_back();

			// 새로 만들어진 노드를 순서에 맞게 tops 에 넣는다. (TODO 우선순위 큐를 이용)
			bool inserted = false;
			vector<BTreeNode*>::iterator iter = tops.begin();
			for(; iter != tops.end(); ++iter)
			{
				if((*iter)->p < n->p)
				{
					tops.insert(iter, n);
					inserted = true;
					break;
				}
			}

			// 가장 빈도가 작은 경우
			if(!inserted) tops.push_back(n);

			numtop--;
		}

		//printf("* 코드맵 생성\n"); 
		BTreeNode *root = tops[0];
		_generate_code(root);

		// 자원 해제 
		_free_btree(root);
		tops.clear();
	}

	// 재귀적으로 트리를 돌아 코드맵 생성
	void _generate_code(BTreeNode *node)
   	{
		static string sequence = "";
		if(node->left)
		{
			sequence += node->lcode;
			_generate_code(node->left);
		}

		if(node->right)
		{
			sequence += node->rcode;
			_generate_code(node->right);
		}

		if(!node->left && !node->right) code_map[node->ch] = sequence;

		int l = (int)sequence.length();
		if(l > 1) sequence = sequence.substr(0, l - 1);
		else sequence = "";
	}

	// 이진트리 제귀적으로 메모리 해제
	void _free_btree(BTreeNode *node)
   	{
		if(node->left)
		{
			_free_btree(node->left);
			delete node->left;
			node->left = NULL;
		}

		if(node->right)
		{
			_free_btree(node->right);
			delete node->right;
			node->right = NULL;
		}
	}
};

int exit_usage()
{
	printf("Usage: ./huffman IN-FILE\n");
	exit(0);
}

int main(int argc, char **argv)
{
	if(argc != 2) exit_usage();

	HuffmanCodec hcodec;

	FILE *fp = fopen(argv[1], "r");
	assert(fp);

	// 입력파일을 한 라인씩 읽어 처리 
	int count = 0;
	while(!feof(fp)) 
	{
		char msg[1024] = {0,};
		fgets(msg, sizeof(msg), fp); 

		char fname_encode[128];
		char fname_decode[128];

		int len = strlen(msg);
		if(len > 1) // 빈칸 무시 
		{
			count++;
			msg[len-1] = 0;
			printf("* Huffman Encoding ... #%d\n", count);
			printf("- 메세지 : %s\n", msg);
			sprintf(fname_encode, "encoded_msg%d.txt", count);	
			hcodec.encode(msg, fname_encode);

			printf("* Huffman Decoding ... #%d\n", count);
			sprintf(fname_decode, "decoded_msg%d.txt", count);	
			hcodec.decode(fname_encode, fname_decode);
		}
     }

    fclose(fp);

	return 0;
}
