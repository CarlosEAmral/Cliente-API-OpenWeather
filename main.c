#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <stdio.h>
#include <string.h>

// Remove acentos e caracteres especiais de uma string UTF-8
void removerAcentos(char *str) {
    if (str == NULL) return;

    // Mapeamento de caracteres acentuados para não acentuados
    unsigned char comAcento[] = "áàâãäÁÀÂÃÄéèêëÉÈÊËíìîïÍÌÎÏóòôõöÓÒÔÕÖúùûüÚÙÛÜçÇ";
    unsigned char semAcento[] = "aaaaaAAAAAeeeeEEEEiiiiIIIIoooooOOOOOuuuuUUUUcC";

    for (size_t i = 0; str[i] != '\0'; i++) {
        unsigned char *p = (unsigned char*)strchr((char*)comAcento, str[i]);
        if (p) {
            size_t pos = p - comAcento;
            str[i] = semAcento[pos];
        }
    }
}


void codificar_url(char *str) {
    char temp[200];
    int j = 0;
    for (int i = 0; str[i] && j < sizeof(temp) - 1; i++) {
        if (str[i] == ' ') {
            temp[j++] = '%';
            temp[j++] = '2';
            temp[j++] = '0';
        } else {
            temp[j++] = str[i];
        }
    }
    temp[j] = '\0';
    strcpy(str, temp);
}


void extrair_coordenadas(const char *arquivo, char *lat, char *lon)
{
    FILE *fp = fopen(arquivo, "r");
    if (!fp)
    {
        printf("Erro ao abrir %s\n", arquivo);
        return;
    }

    char buffer[8192];
    fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[sizeof(buffer) - 1] = '\0';
    fclose(fp);

    char *pos_lat = strstr(buffer, "\"lat\":");
    char *pos_lon = strstr(buffer, "\"lon\":");

    if (pos_lat && pos_lon)
    {
        sscanf(pos_lat, "\"lat\":%[^,],", lat);
        sscanf(pos_lon, "\"lon\":%[^,],", lon);
    }
    else
    {
        printf(" Coordenadas nao encontradas no JSON.\n");
        lat[0] = '\0';
        lon[0] = '\0';
    }
}

void extrair_temperatura(const char *arquivo)
{
    FILE *fp = fopen(arquivo, "r");
    if (!fp)
    {
        printf("Erro ao abrir %s\n", arquivo);
        return;
    }

    char buffer[8192];
    fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[sizeof(buffer) - 1] = '\0';
    fclose(fp);

    char *pos = strstr(buffer, "\"temp\":");
    if (pos)
    {
        float temperatura;
        if (sscanf(pos, "\"temp\":%f", &temperatura) == 1)
        {
            printf(" Temperatura atual: %.2f °C\n", temperatura);
        }
        else
        {
            printf(" Erro ao extrair temperatura.\n");
        }
    }
    else
    {
        printf(" Campo \"temp\" nao encontrado.\n");
    }
}

int main()
{
    char cidade[100];
    char lat[20] = "";
    char lon[20] = "";
    char comando_geo[512];
    char comando_weather[512];
    const char *api_key = "936f211b79c103221fa819509c2281b7"; // Substitua pela sua chave real

    printf("Digite o nome da cidade: ");
    fgets(cidade, sizeof(cidade), stdin);
    cidade[strcspn(cidade, "\n")] = '\0'; 
    
    remover_acentos(cidade);
    printf("%s  %d\n", cidade, cidade[4]);
    codificar_url(cidade); 
    printf("%s", cidade);
    snprintf(comando_geo, sizeof(comando_geo),
             "curl -s \"https://api.openweathermap.org/geo/1.0/direct?q=%s&limit=1&appid=%s\" -o geo.json",
             cidade, api_key);

    if (system(comando_geo) != 0)
    {
        printf(" Erro ao buscar coordenadas.\n");
        return 1;
    }

    extrair_coordenadas("geo.json", lat, lon);

    if (strlen(lat) == 0 || strlen(lon) == 0)
    {
        printf(" Coordenadas invalidas. Encerrando.\n");
        return 1;
    }

    printf(" Coordenadas encontradas: lat=%s, lon=%s\n", lat, lon);

    snprintf(comando_weather, sizeof(comando_weather),
             "curl -s \"https://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s&units=metric\" -o resposta.json",
             lat, lon, api_key);

    if (system(comando_weather) != 0)
    {
        printf(" Erro ao buscar temperatura.\n");
        return 1;
    }

    extrair_temperatura("resposta.json");

    return 0;
}
