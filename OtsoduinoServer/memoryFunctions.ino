void printpgm(char *texto)
{
  char *mem=texto;

  while (pgm_read_byte(mem) != 0x00) /* Comparamos con \0, un terminador */
    Serial.print(pgm_read_byte(mem++));
  Serial.println();
}

