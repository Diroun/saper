#include <vector>

#include "Cell.hpp"
#include "GetCell.hpp"
#include "Global.hpp"

//����� � ��� ��������� �� ������
Cell* get_cell(unsigned char i_x, unsigned char i_y, std::vector<Cell>& i_cells)
{
	return &i_cells[i_x + COLUMNS * i_y];
}