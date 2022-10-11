#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

typedef struct veiculo {
  char cod_cli[12];
  char cod_vei[8];
  char client[50];
  char veiculo[50];
  char dias[4];
} veic_t;

typedef struct remove {
  char cod_cli[12];
  char cod_vei[8];
} reg_id_t;

int clearBuffer() {    
  while ( getchar() != '\n' );
}

int initiateFiles(FILE *p_insere, FILE *p_remove) {
  FILE *fd;
  char registro[120];
  int size_reg;

  veic_t regs_locs_vei[8] = {{"11111111111", "ABC1234", "Cliente-1", "Veiculo-11", "2"},
                             {"22222222222", "ABC1234", "Cliente-2", "Veiculo-11", "8"},
                             {"33333333333", "CDE9874", "Cliente-3", "Veiculo-33", "1"},
                             {"44444444444", "ERT4561", "Cliente-4", "Veiculo-44", "11"},
                             {"11111111111", "ERT4561", "Cliente-1", "Veiculo-44", "2"},
                             {"11111111111", "UJG3574", "Cliente-1", "Veiculo-66", "3"},
                             {"77777777777", "TOP5487", "Cliente-7", "Veiculo-22", "2"},
                             {"88888888888", "IUY3214", "Cliente-8", "Veiculo-88", "2"}};

  for (int i = 0; i < 8; i++) {
    sprintf(registro, "%s|%s|%s|%s|%s|", regs_locs_vei[i].cod_cli, regs_locs_vei[i].cod_vei, regs_locs_vei[i].client, regs_locs_vei[i].veiculo, regs_locs_vei[i].dias);
    size_reg = strlen(registro);

    fwrite(&size_reg, sizeof(int), 1, p_insere);
    fwrite(registro, sizeof(char), size_reg, p_insere);
  }

  //////////////////////////////

  reg_id_t regs_id_list[4] = {{"33333333333", "CDE9874"},
                              {"11111111111", "ERT4561"},
                              {"77777777777", "TOP5487"},
                              {"44444444444", "ERT4561"}};

  for (int i=0; i < 4; i++) {
    sprintf(registro, "%s%s|", regs_id_list[i].cod_cli, regs_id_list[i].cod_vei);
    size_reg = strlen(registro);

    fwrite(&size_reg, sizeof(int), 1, p_remove);
    fwrite(registro, sizeof(char), size_reg, p_remove);
  }
}

int pull_register(FILE *p_insere, char *p_reg) {
  int bytes;

  if (!fread(&bytes, sizeof(int), 1, p_insere)) {
    return 0;
  }
  else {
    fread(p_reg, bytes, 1, p_insere);
    return bytes;
  }
}

int pull_field(char *p_register, int *p_pos, char *p_field) {
  char ch;
  int i = 0;

  p_field[i] = '\0';
  ch = p_register[*p_pos];

  while ((ch != '|') && (ch != '\0'))
  {
    p_field[i] = ch;
    i++;
    ch = p_register[++(*p_pos)];
  }

  ++(*p_pos);
  p_field[i] = '\0';

  return strlen(p_field);
}

void assign_register(FILE *p_insere, veic_t *p_regs_locs_vei, reg_id_t *p_regs_id_list) { 
  int size_reg, num_register = 0;
  char rental_register[120];
  size_reg = pull_register(p_insere, rental_register);

  while (size_reg > 0) {
    char field[50];
    int index = 0, ind_field = 0;

    for (int i = 0; i < size_reg; i++) {
      field[index] = rental_register[i];
      if (field[index] == '|') {

        field[index] = '\0';

        switch (ind_field) {
        case 0:
          strcpy(p_regs_locs_vei[num_register].cod_cli, field);
          break;
        case 1:
          strcpy(p_regs_locs_vei[num_register].cod_vei, field);
          break;
        case 2:
          strcpy(p_regs_locs_vei[num_register].client, field);
          break;
        case 3:
          strcpy(p_regs_locs_vei[num_register].veiculo, field);
          break;
        case 4:
          strcpy(p_regs_locs_vei[num_register].dias, field);
          break;
        }

        ind_field++;
        index = 0;
        continue;
      }

      index++;
    }

    size_reg = pull_register(p_insere, rental_register);
    num_register++;
  }

}

int insert(FILE *p_insere, FILE *p_rents, veic_t *p_regs_locs_vei) {
  int size_reg , option, ind=1;
  char insert_register[120];

  printf("-------------------------------------------------------\n");
  printf("Digite o número da opção que deseja inserir:\n\n");

  for (int i = 0; i < 8; i++) {
    printf("%d - ", i+1);
    if(!(strcmp(p_regs_locs_vei[i].cod_cli, "***"))) {
      printf("***\n"); 
      continue;
    }
    printf("%s |", p_regs_locs_vei[i].cod_cli);
    printf("%s |", p_regs_locs_vei[i].cod_vei);
    printf("%s |", p_regs_locs_vei[i].client);
    printf("%s |", p_regs_locs_vei[i].veiculo);
    printf("%s\n", p_regs_locs_vei[i].dias);
  }

  scanf("%d", &option);
  clearBuffer();

  option--;
  while((option<0 || option>7) || !(strcmp(p_regs_locs_vei[option].cod_cli, "***")) ) {
    printf("Opção inválida, digite novamente: ");
    scanf("%d", &option);
    option--;
    clearBuffer();
  }


  sprintf(insert_register, "%s|%s|%s|%s|%s|", p_regs_locs_vei[option].cod_cli, p_regs_locs_vei[option].cod_vei, p_regs_locs_vei[option].client, p_regs_locs_vei[option].veiculo, p_regs_locs_vei[option].dias);
  size_reg = strlen(insert_register);

  fwrite(&size_reg, sizeof(char), 1, p_rents);
  fwrite(insert_register, sizeof(char), size_reg, p_rents);

  strcpy(p_regs_locs_vei[option].cod_cli, "***"); //Desconsidera o registro já escolhido para ser inserido na lista
  printf("%s \n", p_regs_locs_vei[option].cod_cli);
}

int main() {
  int option;
  FILE *insere, *remove, *rents;
  veic_t regs_locs_vei[8];
  reg_id_t regs_id_list[4];

  if ((insere = fopen("insere.bin", "w+b")) == NULL) {
    printf("Nao foi possivel abrir o arquivo insere.bin");
    return 0;
  }

  if((remove = fopen("remove.bin", "w+b")) == NULL) {
    printf("Nao foi possivel abrir o arquivo remove.bin");
    return 0;
  }

  if(!(rents=fopen("rents.bin", "rb"))) { //Verifica se o arquivo existe
    if((rents = fopen("rents.bin", "w+b")) == NULL) { //Se nao existir será criado
      printf("Nao foi possivel criar o arquivo rents.bin");
      return 0;
    }
  } else { //E se ja existir, vai abrir o arquivo para escrita e leitura
    fclose(rents);

    if((rents = fopen("rents.bin", "r+b")) == NULL) {
      printf("Nao foi possivel abrir o arquivo rents.bin");
      return 0;
    }
  }

  initiateFiles(insere, remove); //Insere os dados nos arquivos de insere.bin e remove.bin
  fseek(insere, 0, 0);
  assign_register(insere, regs_locs_vei, regs_id_list); //Vai pegar os valores do arquivo insere.bin e coloca-los na memoria principal, em um vetor

  printf("-------------------------\n");
  printf("Escolha uma opção: \n");
  printf("1 - Inserir novo aluguel\n");
  printf("2 - Remover aluguel\n");
  printf("3 - Compactação\n");
  printf("4 - sair\n\n");
  printf("informe o número: ");
  scanf("%d", &option);
  clearBuffer();

  while(option != 4) {
    switch (option) {
      case 1: {
        fseek(insere, 0, 0);
        insert(insere, rents, regs_locs_vei);
        break;
      }
      case 2: {

        break;
      }
      case 3: {

        break;
      }
      case 4: {
        printf("saindo...\n");
        break;
      }
      default:
        printf("Opção invalida, por favor digite novamente\n");
    }
    // system("clear");
    printf("-------------------------\n");
    printf("Escolha uma opção: \n");
    printf("1 - Inserir novo aluguel\n");
    printf("2 - Remover aluguel\n");
    printf("3 - Compactação\n");
    printf("4 - sair\n\n");
    printf("informe o número: ");
    scanf("%d", &option);
    clearBuffer();
  }

  fclose(insere);
  fclose(remove);
  fclose(rents);
}