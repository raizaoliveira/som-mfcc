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
			puts("reconhece por som");
		}
		free(mfcc_frames);
		free(frames);
	}
	if (flag)
		printf("\n");
	free(buffer);
	
}


