#include "link.h"


void get_word(int flag)
{
	unsigned int size = 0, n = 0, i = 0;
	char ans[500];
	int frame_n;
	void *buffer = NULL;
	frame *frames = NULL;
	mfcc_frame *mfcc_frames = NULL;
	voice_signal *signals = NULL;
	printf("\nFale agora e pressione enter\n");
	capture_start(NULL);
	getchar();
	getchar();
	capture_stop(&size, &buffer);
	n = split(buffer, size / 2, &signals);
	for (i = 0; i < n; i++)
	{
		frame_n = make_frames_hamming(signals[i].buffer, signals[i].number, &frames);
		mfcc_frames = malloc(sizeof(mfcc_frame) * frame_n);
		mfcc_features(frames, frame_n, mfcc_frames);
		if (!flag)
		{
			record(NULL, signals[i].buffer, signals[i].number * 2);
			printf("Escreva a palavra: ");
			scanf("%s", (char *)ans);
			if (!(ans[0] == 'x' && ans[1] == '\0'))
			{
				new_word(mfcc_frames, frame_n, (char *)ans);
				chdir("waves");
				char *path = malloc(strlen(ans) + 5);
				char *ext = ".wav";
				memcpy(path, ans, strlen(ans));
				memcpy(path + strlen(ans), ext, 5);
				write_pcm(signals[i].buffer, signals[i].number * 2, path);
				free(path);
				chdir("..");
			}
		}
		else
		{
			/*laço de reconhecimento*/	
			puts("analisando dados...");
			word *words = malloc(sizeof(word));
			double best = 1e10;
			char *name = NULL;
			void **word_adresses;
			unsigned int n = 0, i = 0, count = 0;
			if ((n = get_list(words)))
			{
				word_adresses = malloc(n * sizeof(void *));
				while (words != NULL)
				{
					double now = compare(mfcc_frames, frame_n, words->frames, words->n);
					word_adresses[count++] = words;
					if (now < best)
					{
						best = now;
						name = words->name;
					}
					words = words->next;
				}
				for (i = 0; i < count; i++)
					free(word_adresses[i]);
				free(word_adresses);
			}
			if (best < 3.5)
				printf("%f %s", best, name);
		}
		free(mfcc_frames);
		free(frames);
	}
	if (flag)
		printf("\n");
	free(buffer);
	
}


