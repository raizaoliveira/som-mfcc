#include "preprocessing.h"

#define LEN_PART_MIN 10
#define LEN_MIN 20

struct voice_signal_list
{
	voice_signal vs;
	struct voice_signal_list *next;
};

/*frequencias centrais em Hz*/
static int frequency[QTD_FILTER] = {100, 200, 300, 400,
				500, 600, 700, 800, 900, 1000, 1148,
				1318, 1514, 1737, 1995, 2291, 2630, 3020, 3467, 4000};


int split(int16_t *buffer, unsigned int n, voice_signal **signals)
{
	unsigned int i = 0, j = 0, k = 0, start_pos = 0, len_sig = 0, len_nothing = 0, count = 0;
	char started = 0;
	double means[n / N], sds[n / N];
	double sdmean = 0;
	void **free_list = NULL;
	struct voice_signal_list *head = malloc(sizeof(struct voice_signal_list));
	struct voice_signal_list *now = head;

	head->next = NULL;

	// Calcula os desvios padrão de cada quadro
	for (i = 0; i < n / N; i++)
	{
		means[i] = 0;
		for (j = 0; j < N; j++)
			means[i] += ((int16_t *)buffer)[i * N + j];
		means[i] /= N;
		sds[i] = 0;
		for (j = 0; j < N; j++)
			sds[i] += pow(((int16_t *)buffer)[i * N +j] - means[i], 2);
		sds[i] = sqrt(sds[i]) / (N - 1);
		sdmean += sds[i];
	}
	sdmean /= (n / N);

	// comparação dos desvios-padrão do quadro atual com o desvio padrão médio, para descobrir onde os sinais ocorrem
	for (i = 0; i < n / N; i++)
	{
		//sinal disponivel
		if (sdmean < sds[i])
		{
			//inicializacao sinal start
			if (!started)
			{
				started = 1;
				start_pos = i;
				len_nothing = 0;
			}
			len_sig++;
		}
		//sinal muito fraco
		else
			// (sinal - curto silêncio - sinal) deve ser interpretado como um único sinal
			if (len_sig > LEN_PART_MIN)
			{
				len_nothing = 0;
				len_sig = 0;
			}
			else
			{
				// Duração do silêncio
				len_nothing++;
				if (started && i - LEN_MIN > start_pos && (len_nothing > 2 * LEN_MIN || i == n / N - 1))
				{
					 // Cria uma nova entrada 
					now->next = malloc(sizeof(struct voice_signal_list));
					now = now->next;
					now->next = NULL;
					now->vs.number = (i - start_pos) * N;
					now->vs.buffer = malloc(sizeof(int16_t) * now->vs.number);

					for (j = start_pos; j < i; j++)
					{
						for (k = 0; k < N; k++)
							now->vs.buffer[(j - start_pos) * N + k] = ((int16_t *)buffer)[j * N + k];
					}

					count++;
					started = 0;
				}
			}
	}

	//Preenche o sinal parâmetro com os sinais de voz reconhecidos
	*signals = malloc(sizeof(voice_signal) * count);
	free_list = malloc(sizeof(void *) * count);
	now = head;
	for (i = 0; i < count; i++)
	{
		now = now->next;
		free_list[i] = now;
		(*signals)[i] = now->vs;
	}
	for (i = 0; i < count; i++)
		free(free_list[i]);
	free(free_list);
	free(head);

	return count;
}

unsigned int make_frames_hamming(int16_t *buffer, unsigned int n, frame **frames)
{
	unsigned int id_frame = (n / (N - OVERLAY)) - 1;
	comp *result = malloc(sizeof(comp) * id_frame * N);
	comp *data = malloc(sizeof(comp) * id_frame * N);
	unsigned int pos = 0, i = 0, j = 0;
	double pi = 4 * atan(1.0);

	*frames = malloc(sizeof(frame) * id_frame);
	for (i = 0; i < id_frame; i++)
	{
		pos = (i + 1) * (N - 64);
		for (j = 0; j < N; j++)
		{
			data[i * N + j].real = buffer[pos + j] * (0.54 + 0.46 * cos(2 * pi * (j - N / 2) / N));
			data[i * N + j].imag = 0;
		}
	}
	for (i = 0; i < id_frame; i++)
		fft(data + i * N, result + i * N, N);

	for (i = 0; i < id_frame; i++)
		for (j = 0; j < N; j++)
			(*frames)[i].magnitudes[j] = sqrt(pow(result[i * N + j].real, 2) + pow(result[i * N + j].imag, 2));

	for (i = 0; i < id_frame; i++)
	{
		double sig = 0;
		for (j = 0; j < N; j++)
			sig += (*frames)[i].magnitudes[j];
		sig /= N;
		for (j = 0; j < N; j++)
			(*frames)[i].magnitudes[j] /= sig;
	}
	free(data);
	free(result);
	return id_frame;
}

void fft(comp *time, comp *freq, unsigned int n)
{
	double pi = 4 * atan(1.0);
	unsigned int i = 0, j = 0, k = 0;
	unsigned int bits = 0;

	if (n == 1)
	{
		freq[0] = time[0];
		return;
	}

	/*bits necessários para a representação binária de n*/
	for (i = n; i > 1; i /= 2)
		bits++;

	for (i = 0; i < n; i++)
	{
		int reversed = 0;
		for (j = 0; j < bits; j++)
			reversed |= (((i >> j) % 2) << (bits - j - 1));
		freq[reversed] = time[i];
	}

	for (i = bits; i > 0; i--)
	{
		unsigned int length_groups = (n >> (i - 1));
		unsigned int num_groups = n / length_groups;
		for (j = 0; j < num_groups; j++)
		{
			for (k = 0; k < length_groups / 2; k++)
			{
				unsigned int off1 = length_groups * j + k;
				unsigned int off2 = off1 + length_groups / 2;
				comp off1_val, off2_val;
				double cosine = cos((-2 * pi * k) / length_groups);
				double sine = sin((-2 * pi * k) / length_groups);
				off1_val.real = freq[off1].real + (freq[off2].real * cosine - freq[off2].imag * sine);
				off1_val.imag = freq[off1].imag + (freq[off2].imag * cosine + freq[off2].real * sine);
				off2_val.real = freq[off1].real - (freq[off2].real * cosine - freq[off2].imag * sine);
				off2_val.imag = freq[off1].imag - (freq[off2].imag * cosine + freq[off2].real * sine);
				freq[off1] = off1_val;
				freq[off2] = off2_val;
			}
		}
	}
	for (i = 0; i < n; i++)
	{
		freq[i].real /= n;
		freq[i].imag /= n;
		freq[i].imag *= -1;
	}
}

void mfcc_features(frame *frames, unsigned int n, mfcc_frame *mfcc_frames)
{
	unsigned int i = 0, j = 0, k = 0;
	double out_filter[QTD_FILTER];
	double filterSpectrum[QTD_FILTER][N] = {{0}};
	double c0 = sqrt(1.0 / QTD_FILTER);
	double cn = sqrt(2.0 / QTD_FILTER);
	double pi = 4 * atan(1.0);

	for (i = 0; i < QTD_FILTER; i++)
	{
		double maxFreq = 0, minFreq = 0, centerFreq = 0;

		if (i == 0)
			minFreq = frequency[0] - (frequency[1] - frequency[0]);
		else
			minFreq = frequency[i - 1];
		centerFreq = frequency[i];

		if (i == QTD_FILTER - 1)
			maxFreq = frequency[QTD_FILTER - 1] + (frequency[QTD_FILTER - 1] - frequency[QTD_FILTER - 2]);
		else
			maxFreq = frequency[i + 1];

		for (j = 0; j < N; j++)
		{
			double freq = 1.0 * j * WAVE_SAMPLE_RATE / N;
			if (freq > minFreq && freq < maxFreq)
				if (freq < centerFreq)
					filterSpectrum[i][j] = 1.0 * (freq - minFreq) / (centerFreq - minFreq);
				else
					filterSpectrum[i][j] = 1 - 1.0 * (freq - centerFreq) / (maxFreq - centerFreq);
			else
				filterSpectrum[i][j] = 0;
		}
	}

	/*Calcula os coeficientes MFCC para cada quadro.*/
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < QTD_FILTER; j++)
		{
			out_filter[j] = 0;
			for (k = 0; k < N; k++)
				out_filter[j] += frames[i].magnitudes[k] * filterSpectrum[j][k];
		}
		for (j = 0; j < N_MFCC; j++)
		{
			mfcc_frames[i].features[j] = 0;

			for (k = 0; k < QTD_FILTER; k++)
				mfcc_frames[i].features[j] += log(fabs(out_filter[k]) + 1e-10) * cos((pi * (2 * k + 1) * j) / (2 * QTD_FILTER));

			if (j)
				mfcc_frames[i].features[j] *= cn;
			else
				mfcc_frames[i].features[j] *= c0;
		}
	}
}
