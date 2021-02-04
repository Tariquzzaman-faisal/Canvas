#include "global.hpp"

float pi = (float)acos(-1);

void canvas_draw(sf::RenderWindow& artBoard)
{
	//std::cout << "Currently drawing\n";
	if (!last_cleared)
	{
		for (auto i = 0; i < (int)vertices.size() - undo_count; i++)
		{
			artBoard.draw(vertices[i]);
		}
	}
}

void mouseToggle(sf::Event& evnt)
{
	if (evnt.type == sf::Event::MouseButtonPressed)
	{
		if (undo_count > 0)
		{
			while (undo_count)
			{
				vertices.pop_back();
				--lines_number;
				--undo_count;
			}
		}

		if (evnt.mouseButton.button == sf::Mouse::Left)
		{
			undo_count = 0;
			if (last_cleared)
			{
				vertices.clear();
				lines_number = -1;
				last_cleared = false;
			}

			if (penSelected || brushSelected || eraserSelected)
			{
				vertices.push_back(sf::VertexArray(sf::TriangleStrip));
				lines_number++;
			}

			mousePressedDown = true;
		}
	}
	if (evnt.type == sf::Event::MouseButtonReleased)
	{
		if (evnt.mouseButton.button == sf::Mouse::Left)
		{
			mousePressedDown = false;
		}
	}
}

void brushConnect(sf::Vector2i newPos, sf::Vector2i lastPos, float radius, sf::Color col)
{
	float curr_slope, mult;

	if (newPos.y != lastPos.y)
	{
		curr_slope = -(float)(newPos.x - lastPos.x) / (newPos.y - lastPos.y);
		mult = sqrt((radius * radius) / (curr_slope * curr_slope + 1));

		vertices[lines_number].append(sf::Vertex(getCoordinates({ newPos.x - mult, newPos.y - curr_slope * mult }), col));
		vertices[lines_number].append(sf::Vertex(getCoordinates({ newPos.x + mult, newPos.y + curr_slope * mult }), col));
	}

	else
	{
		curr_slope = 1; // m = 1/0
		vertices[lines_number].append(sf::Vertex(getCoordinates({ newPos.x + radius, newPos.y - radius }), col));
		vertices[lines_number].append(sf::Vertex(getCoordinates({ newPos.x + radius, newPos.y + radius }), col));
	}
}

void rectangleConnect(sf::Vector2f newPos, sf::Vector2f lastPos, float radius, sf::Color col, int pos)
{
	float curr_slope, mult;

	if (newPos.y != lastPos.y)
	{
		curr_slope = -(float)(newPos.x - lastPos.x) / (newPos.y - lastPos.y);
		mult = sqrt((radius * radius) / (curr_slope * curr_slope + 1));

		vertices[lines_number][pos * 2] = (sf::Vertex(getCoordinates({ newPos.x + mult, newPos.y + curr_slope * mult }), col));
		vertices[lines_number][pos * 2 + 1] = (sf::Vertex(getCoordinates({ newPos.x - mult, newPos.y - curr_slope * mult }), col));
	}

	else
	{
		curr_slope = 1; // m = 1/0
		vertices[lines_number][pos * 2] = (sf::Vertex(getCoordinates({ newPos.x + radius, newPos.y - radius }), col));
		vertices[lines_number][pos * 2 + 1] = (sf::Vertex(getCoordinates({ newPos.x + radius, newPos.y + radius }), col));
	}
}

void circleConnect(sf::Vector2f center, float radius, sf::Color col)
{
	sf::Vector2i newPos, oldPos, firstPos;
	//std::cout << center.x << " " << center.y << '\n';
	int points = (int)ceil(radius * 10);
	float degInc = 2 * pi / points;
	vertices[lines_number].clear();
	for (float degree = 0; degree < 2 * pi; degree += degInc) {
		float circle_x = radius * cos(degree);
		float circle_y = radius * sin(degree);
		sf::Vector2f point;
		point.x = center.x + circle_x;
		point.y = center.y + circle_y;
		newPos = (sf::Vector2i)point;
		if (degree == 0)
			firstPos = newPos;
		else
			brushConnect(newPos, oldPos, brushSize, col);
		oldPos = newPos;
	}

	brushConnect(oldPos, firstPos, brushSize, col);
}

sf::Vector2f getCoordinates(sf::Vector2f oldCord)
{
	sf::Vector2f newCord;
	if (zoomSelected)
	{
		newCord.x = (oldCord.x / 3) + zoomCordX - artBoardWidth / 6;
		newCord.y = (oldCord.y / 3) + zoomCordY - artBoardHeight / 6;
	}
	else
	{
		newCord = oldCord;
	}
	return newCord;
}

sf::VertexArray fillSquare(sf::Vector2f center, float radius, sf::Color col)
{
	sf::VertexArray square;
	square.setPrimitiveType(sf::TriangleFan);
	square.append(sf::Vertex(getCoordinates(sf::Vector2f(center)), col));
	square.append(sf::Vertex(getCoordinates(sf::Vector2f(center.x - brushSize, center.y + brushSize)), col));
	square.append(sf::Vertex(getCoordinates(sf::Vector2f(center.x + brushSize, center.y + brushSize)), col));
	square.append(sf::Vertex(getCoordinates(sf::Vector2f(center.x + brushSize, center.y - brushSize)), col));
	square.append(sf::Vertex(getCoordinates(sf::Vector2f(center.x - brushSize, center.y - brushSize)), col));
	square.append(sf::Vertex(getCoordinates(sf::Vector2f(center.x - brushSize, center.y + brushSize)), col));

	return square;
}