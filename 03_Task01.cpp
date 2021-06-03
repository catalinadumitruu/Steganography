#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 100

struct Pixel
{
	char red;
	char green;
	char blue;
};

bool isBitOn(char letter, int index)
{
	return (letter >> index & 1) ? true : false;
}

bool isBitOnXor(char letter, int index, char forXor, int passIndex)
{
	return (((letter >> index) ^ forXor) & 1) ? true : false;
}

void encode() {
	FILE* fCover = fopen("Image.bmp", "rb");
	FILE* fData = fopen("Data.txt", "r");
	FILE* fStega = fopen("Encoded.bmp", "wb");

	fseek(fCover, 0, SEEK_SET);
	char buffer[BUFFER_SIZE];
	fread(buffer, 54, 1, fCover);
	fwrite(buffer, 54, 1, fStega);

	char letter;
	int bitIndex = 0;
	bool encoded = false;
	bool textEnded = false;
	Pixel* ptrPixel = (Pixel*)malloc(sizeof(Pixel));

	fread(&letter, 1, 1, fData);

	while (fread(ptrPixel, 3, 1, fCover) != 0) {
		if (!encoded) {
			if (isBitOn(letter, 7 - bitIndex))
				ptrPixel->red |= 1;
			else
				ptrPixel->red &= ~1;

			fwrite(ptrPixel, 3, 1, fStega);

			bitIndex++;

			if (textEnded && bitIndex == 8)
				encoded = true;

			if (bitIndex == 8) {
				bitIndex = 0;
				fread(&letter, 1, 1, fData);

				if (feof(fData)) {
					textEnded = true;
					letter = '\0';
				}
			}
		}
		else
			fwrite(ptrPixel, 3, 1, fStega);
	}
	fclose(fCover);
	fclose(fData);
	fclose(fStega);
}

int char_to_int(char character)
{
	char number = character;
	char bits[8];
	for (int i = 0; i < 8; i++)
	{
		bits[i] = (number >> i) & 1;

	}
	//printf("%d\n", bits[0]);
	return  bits[0];
}

void decode() {
	FILE* fStega = fopen("Encoded.bmp", "rb");
	FILE* fDataRecovered = fopen("DataRecovered.txt", "w");

	fseek(fStega, 0, SEEK_SET);
	char buffer[BUFFER_SIZE];
	fread(buffer, 54, 1, fStega);

	Pixel* pixel = (Pixel*)malloc(sizeof(Pixel)); // 1 pixel
	int letterIndex = 0;
	bool decoded = false;
	bool picEnded = false;
	bool textEnded = false;
	int letter[8];
	int i = 0;
	
	while (fread(pixel, 3, 1, fStega) != 0) {
		if (!decoded) {
		
			letter[letterIndex++] = char_to_int(pixel->red);

			if (letterIndex == 8 && textEnded) {
				decoded = true;
			}

			if (letterIndex == 8) {
				letterIndex = 0;

				int multiplier = 1;
				int bin = 0;
				for (int i = 7; i >= 0; --i) {
					bin += (multiplier * letter[i]);
					multiplier *= 2;
				}
				
				if (feof(fStega) || letter == NULL || bin == 0) {
					textEnded = true;
				}
				else {
					if (bin >= 32 && bin <= 122) {
						//printf("%d\n", bin);
						char letter = bin;
						fwrite(&letter, 1, 1, fDataRecovered);
					}
				}
			}
		}
	}
	fclose(fStega);
	fclose(fDataRecovered);
}

void encodeWithPass(const char* password) {
	FILE* fCover = fopen("Image.bmp", "rb");
	FILE* fData = fopen("Data.txt", "r");
	FILE* fStega = fopen("EncodedWithPass.bmp", "wb");

	fseek(fCover, 0, SEEK_SET);
	char buffer[BUFFER_SIZE];
	fread(buffer, 54, 1, fCover);
	fwrite(buffer, 54, 1, fStega);

	char letter;
	int bitIndex = 0;
	bool encoded = false;
	bool textEnded = false;
	Pixel* ptrPixel = (Pixel*)malloc(sizeof(Pixel));
	int passSize = strlen(password);
	int i = 0;

	fread(&letter, 1, 1, fData);

	while (fread(ptrPixel, 3, 1, fCover) != 0) {
		if (!encoded) {
			char forXor;
			if (i < passSize) {
				forXor = password[i++];
			}
			else {
				i = 0;
				forXor = password[i++];
			}

			if (isBitOnXor(letter, 7 - bitIndex, forXor, passSize - i))
				ptrPixel->red |= 1;
			else
				ptrPixel->red &= ~1;

			fwrite(ptrPixel, 3, 1, fStega);

			bitIndex++;

			if (textEnded && bitIndex == 8)
				encoded = true;

			if (bitIndex == 8) {
				bitIndex = 0;
				fread(&letter, 1, 1, fData);

				if (feof(fData)) {
					textEnded = true;
					letter = '\0';
				}
			}
			
		}
		else
			fwrite(ptrPixel, 3, 1, fStega);
	}
	fclose(fCover);
	fclose(fData);
	fclose(fStega);
}

int char_to_int_withXOR(char character, char forXor)
{
	char bits[8];
	for (int i = 0; i < 8; i++)
	{
		bits[i] = ((character >> i) ^ (forXor)) & 1;
	}
	return  bits[0];
}

void decodeWithPass(const char* password) {
	FILE* fStega = fopen("EncodedWithPass.bmp", "rb");
	FILE* fDataRecovered = fopen("DataRecoveredWithPass.txt", "w");

	fseek(fStega, 0, SEEK_SET);
	char buffer[BUFFER_SIZE];
	fread(buffer, 54, 1, fStega);

	Pixel* pixel = (Pixel*)malloc(sizeof(Pixel)); // 1 pixel
	int letterIndex = 0;
	bool decoded = false;
	bool picEnded = false;
	bool textEnded = false;
	int Xoredletter[8];
	int letter[8];
	int passSize = strlen(password);
	int i = 0;
	int j = 0;

	while (fread(pixel, 3, 1, fStega) != 0) {
		if (!decoded) {
			char forXor;
			if (i < passSize) {
				forXor = password[i++];
			}
			else {
				i = 0;
				forXor = password[i++];
			}

			letter[letterIndex++] = char_to_int_withXOR(pixel->red, forXor);

			if (letterIndex == 8 && textEnded) {
				decoded = true;
			}

			if (letterIndex == 8) {
				letterIndex = 0;

				int multiplier = 1;
				int bin = 0;
				for (int i = 7; i >= 0; --i) {
					bin += (multiplier * letter[i]);
					multiplier *= 2;
				}

				if (feof(fStega) || letter == NULL || bin == 0) {
					textEnded = true;
				}
				else {
					if (bin >= 32 && bin <= 122) {
						//printf("%d\n", bin);
						char letter = bin;
						fwrite(&letter, 1, 1, fDataRecovered);
					}
				}
			}
		}
	}
	fclose(fStega);
	fclose(fDataRecovered);
}

void main()
{
	encode();
	decode();

	char pass[20];
	printf("Please provide password for encoding and decoding the text into the image\n");
	fgets(pass, 20, stdin);

	encodeWithPass(pass);
	decodeWithPass(pass);
}
