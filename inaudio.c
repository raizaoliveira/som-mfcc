

#include "inaudio.h"
#define FRAMES_IN_BLOCK 0x1000
#define BLOCKSIZE (FRAMES_IN_BLOCK * (WAVE_BITS_PER_SAMPLE / 8))
#define PCM_FORMAT SND_PCM_FORMAT_S16_LE

struct block
{
	void *buf;
	struct block *next;
};


struct block *head = NULL;

snd_pcm_t *handle = NULL;
int finish = 2;
unsigned int blocknumber = 0;


/*Arquivo atualmente aberto*/
static FILE *f = NULL;
/*cabeçalho WAVE*/
static char header[45] = "RIFF1024WAVEfmt 10241212102410241212data1024";


/*funcao que captura o audio -> buffer
Verificação dos parametros de hardware
*/
int capture_start(const char *device)
{
	printf("funcao-> capture_start\n");
	unsigned int freq = WAVE_SAMPLE_RATE;
	int freq_adjust_direction = 1;
	snd_pcm_hw_params_t *hw_params = NULL;
	pthread_t t;

	if (snd_pcm_open(&handle, !device ? "default" : device, SND_PCM_STREAM_CAPTURE, 0) < 0)
	{
		fprintf(stderr, "Nao foi possivel abrir dispositivo %s\n", !device ? "default" : device);
		return -1;
	}

	if (snd_pcm_hw_params_malloc(&hw_params) < 0)
	{
		fprintf(stderr, "Falha ao alocar memória para os parâmetros de hardware\n");
		return -2;
	}

	if (snd_pcm_hw_params_any(handle, hw_params) < 0)
	{
		fprintf(stderr, "Falha ao obter os parâmetros de hardware padrão\n");
		return -3;
	}

	if (snd_pcm_hw_params_set_access(handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
	{
		fprintf(stderr, "Definição erro de leitura - \\ acesso de gravação\n");
		return -4;
	}
	if (snd_pcm_hw_params_set_format(handle, hw_params, PCM_FORMAT) < 0)
	{
		fprintf(stderr, "Erro formato de definição PCM\n");
		return -5;
	}
	if (snd_pcm_hw_params_set_rate_near(handle, hw_params, &freq, &freq_adjust_direction) < 0)
	{
		fprintf(stderr, "Falha ao definir a freqüência\n");
		return -6;
	}
	if (snd_pcm_hw_params_set_channels(handle, hw_params, WAVE_CHANNELS) < 0)
	{
		fprintf(stderr, "Falha ao definir o número de canais\n");
		return -7;
	}

	if (snd_pcm_hw_params(handle, hw_params) < 0)
	{
		fprintf(stderr, "Erro ao definir os parâmetros de hardware\n");
		return -8;
	}

	snd_pcm_hw_params_free(hw_params);

	if (snd_pcm_prepare(handle) < 0)
	{
		fprintf(stderr, "Erro no handle\n");
		return -9;
	}

	finish = 0;
	/*Pthread para iniciar a gravação
	A função capture() iniciara em 
	trhead paralela a da execução principal

	*/
	if (pthread_create(&t, NULL, (void *)capture, NULL) < 0)
	{
		fprintf(stderr, "Falha ao iniciar tópicos de gravação\n");
		return -10;
	}
	return 0;
}

void capture(void)
{
	printf("funcao-> capture\n");
	struct block *now = NULL;
	int r;
	blocknumber = 0;
	head = malloc(sizeof(struct block));
	head->buf = NULL;
	head->next = NULL;
	now = head;
	while (!finish)
	{
		unsigned int framenumber = FRAMES_IN_BLOCK;
		blocknumber++;
		now->next = malloc(sizeof(struct block));
		now = now->next;
		now->buf = malloc(BLOCKSIZE);
		now->next = NULL;
		do
		{
			r = snd_pcm_readi(handle, now->buf + (FRAMES_IN_BLOCK - framenumber) * (WAVE_BITS_PER_SAMPLE / 8), framenumber);
			if (r < 0)
				fprintf(stderr, "AVISO: Erro ao ler Áudio\n");
			else
				framenumber -= r;
		} while (framenumber > 0);
	}
	finish = 2;
}

void capture_stop(unsigned int *size, void **buffer)
{
	printf("funcao-> capture_stop \n");
	unsigned int i = 0;
	struct block *now = NULL;
	void **adresses = NULL;

	finish = 1;
	while (finish != 2);

	snd_pcm_close(handle);
	*buffer = malloc(BLOCKSIZE * blocknumber);
	*size = BLOCKSIZE * blocknumber;
       	adresses = malloc(blocknumber * sizeof(void *));

	now = head;
	for (i = 0; i < blocknumber; i++)
	{
		now = now->next;
		adresses[i] = now;
		memcpy(*buffer + (i * BLOCKSIZE), now->buf, BLOCKSIZE);
		free(now->buf);
	}
	for (i = 0; i < blocknumber; i++)
		free(adresses[i]);
	free(head);
	free(adresses);
}

/*funcao para gravacao de audio -> disco*/
int record(const char *device, const void *buffer, const unsigned int size)
{
	printf("funcao-> record\n");
	int r = 0;
	unsigned int freq = WAVE_SAMPLE_RATE;
	unsigned int framenumber = size / (WAVE_BITS_PER_SAMPLE / 8);
	snd_pcm_hw_params_t *hw_params = NULL;

	if (snd_pcm_open(&handle, !device ? "default" : device, SND_PCM_STREAM_PLAYBACK, 0) < 0)
	{
		fprintf(stderr, "Dispositivo não foi possível abrir %s \n", !device ? "default" : device);
		return -1;
	}

	if (snd_pcm_hw_params_malloc(&hw_params) < 0)
	{
		fprintf(stderr, "Falha ao alocar memória para os parâmetros de hardware\n");
		return -2;
	}

	if (snd_pcm_hw_params_any(handle, hw_params) < 0)
	{
		fprintf(stderr, "Falha ao obter os parâmetros de hardware padrão\n");
		return -3;
	}

	if (snd_pcm_hw_params_set_access(handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
	{
		fprintf(stderr, "Definição erro de leitura - \\ acesso de gravação\n");
		return -4;
	}
	if (snd_pcm_hw_params_set_format(handle, hw_params, PCM_FORMAT) < 0)
	{
		fprintf(stderr, "Erro na definicao de formato PCM\n");
		return -5;
	}
	if (snd_pcm_hw_params_set_rate_near(handle, hw_params, &freq, 0) < 0)
	{
		fprintf(stderr, "Erra na definicao da frequencia\n");
		return -6;
	}
	if (snd_pcm_hw_params_set_channels(handle, hw_params, WAVE_CHANNELS) < 0)
	{
		fprintf(stderr, "Falha ao definir o número de canais\n");
		return -7;
	}

	if (snd_pcm_hw_params(handle, hw_params) < 0)
	{
		fprintf(stderr, "Falha ao definir parametros de hardware\n");
		return -8;
	}

	snd_pcm_hw_params_free(hw_params);

	if (snd_pcm_prepare(handle) < 0)
	{
		fprintf(stderr, "Erro preparando Handles\n");
		return -9;
	}

	do
	{
		r = snd_pcm_writei(handle, buffer + (size - framenumber * (WAVE_BITS_PER_SAMPLE / 8)), framenumber);
		if (r < 0)
			fprintf(stderr, "AVISO: Falha ao gravar Áudio\n");
		else
			framenumber -= r;
	} while (framenumber > 0);

	snd_pcm_close(handle);
	return 0;
}

int write_pcm(const void *buffer, const unsigned int size, const char *path)
{
	FILE *file = NULL;
	printf("funcao-> write_pcm\n");

	if (path != NULL)
		file = fopen(path, "w");
	else
	{
		rewind(f);
		file = f;
	}
	if (file == NULL)
		return -1;

	prepare_header(size + 44);

	fwrite(header, 44, 1, file);
	fwrite(buffer, size, 1, file);

	if (path != NULL)
		fclose(file);
	return 0;
}

/*
Prepara o cabeçalho wave
  * Preenche o cabeçalho comparação com os valores válidos, que são especificados 
em wave.h e o arquivo e bloco de tamanho correto, que é derivado dos parâmetros.
  * (Const unsigned int) tamanho do arquivo
*/
void prepare_header(const unsigned int size)
{
	printf("funcao-> prepare_header\n");
	int values[9] = {size - 8, 16, WAVE_FORMAT, WAVE_CHANNELS, WAVE_SAMPLE_RATE, WAVE_BYTES_PER_SECOND, WAVE_FRAME_SIZE, WAVE_BITS_PER_SAMPLE, size - 44};

	memcpy(header + 4, values, 4);
	memcpy(header + 16, values + 1, 4);
	memcpy(header + 20, values + 2, 2);
	memcpy(header + 22, values + 3, 2);
	memcpy(header + 24, values + 4, 4);
	memcpy(header + 28, values + 5, 4);
	memcpy(header + 32, values + 6, 2);
	memcpy(header + 34, values + 7, 2);
	memcpy(header + 40, values + 8, 4);

}




