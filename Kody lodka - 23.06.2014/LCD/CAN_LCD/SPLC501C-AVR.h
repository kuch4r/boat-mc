
#include <inttypes.h>

#define CS_HIGH PORT(SPI_CS_PORT) |= (_BV(SPI_CS))
#define CS_LOW PORT(SPI_CS_PORT) &= ~(_BV(SPI_CS))

#define A0_HIGH PORT(SPI_A0_PORT) |= (_BV(SPI_A0))
#define A0_LOW PORT(SPI_A0_PORT) &= ~(_BV(SPI_A0))

void Tab_GoTo(unsigned char, unsigned char);
void GLCD_InitializePorts(void);
uint16_t GLCD_Get_x(void);
uint16_t GLCD_Get_y(void);