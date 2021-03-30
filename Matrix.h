#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <functional>
#include <iomanip> 

using namespace std;

namespace matrix 
{
	template <class T>
	class Matrix
	{
	private:
		T* matrix = nullptr;
		size_t n = 0, m = 0, id = 0;

		static bool debug;
		static size_t static_id;

		class Row 
		{
		private:
			size_t index;
			const Matrix* owner;
		public:
			/* Конструктор для строки */
			Row(const Matrix* owner, size_t i)
			{
				this->owner = owner;
				this->index = i;
			}

			/* Перегрузка оператора [] */
			T operator[](size_t x) const
			{
				if (x >= this->owner->m)
				{
					throw "Выход за пределы.";
				}
				return *(this->owner->matrix + this->index * this->owner->n + x);
			}
			T& operator[](size_t x)
			{
				if (x >= this->owner->m)
				{
					throw "Выход за пределы.";
				}
				return *(this->owner->matrix + this->index * this->owner->n + x);
			}
		};
	public:
		/* Конструктор */
		Matrix(size_t n, size_t m, T* matrix = nullptr)
		{
			this->id = ++static_id;

			if (n != 0 && m != 0)
			{
				this->matrix = new T[n * m]();
				this->n = n;
				this->m = m;
			}

			if (matrix != nullptr)
			{
				copy(matrix, matrix + m * n, this->matrix);
			}

			if (debug)
			{
				cout << "Конструктор " << this->id << endl;
			}
		}

		/* Конструктор с параметрами размера матрицы */
		Matrix(size_t n) : Matrix::Matrix(n, n) {}

		/* Конструктор, принимающий лямбда-выражение */
		Matrix(size_t n, size_t m, T(*func)(size_t, size_t)) : Matrix(n, m)
		{
			for (size_t i = 0; i < n; i++)
			{
				for (size_t j = 0; j < m; j++)
				{
					*(this->matrix + i * m + j) = func(i, j);
				}
			}
		}

		/* Конструктор по-умолчанию */
		Matrix() : Matrix(0) {}

		/* Деструктор */
		~Matrix()
		{
			if (!this->is_empty())
			{
				delete this->matrix;
				this->matrix = nullptr;
			}
			if (debug)
			{
				cout << "Деструктор " << this->id << endl;
			}
		}

		/* Копирующий конструктор */
		Matrix(const Matrix& other) : Matrix()
		{
			*this = other;
		}

		/* Копирующий оператор = */
		const Matrix& operator=(const Matrix& other)
		{
			if (debug)
			{
				cout << "Конструктор копирования - копируем " << this->id << " <- " << other.id << endl;
			}
			if (this == &other)
			{
				return *this;
			}

			size_t size = other.get_size();

			if (this->get_size() != size)
			{
				if (this->matrix != nullptr) delete[] this->matrix;
				this->matrix = size > 0 ? new T[size] : nullptr;
			}

			this->m = other.m;
			this->n = other.n;

			if (size > 0)
			{
				copy(other.matrix, other.matrix + size, this->matrix);
			}

			return *this;
		}

		/* Перемещающий конструктор */
		Matrix(Matrix&& other) noexcept : Matrix()
		{
			*this = move(other);
		}

		/* Перемещающий оператор = */
		Matrix& operator=(Matrix&& other) noexcept
		{
			if (debug)
			{
				cout << "Конструктор перемещения " << other.id << endl;
			}
			if (this == &other)
			{
				return *this;
			}

			this->make_null();
			std::swap(this->matrix, other.matrix);
			std::swap(this->n, other.n);
			std::swap(this->m, other.m);

			return *this;
		}

		/* Сделать матрицу пустой */
		Matrix& make_null()
		{
			this->m = this->n = 0;

			if (!this->is_empty())
			{
				delete[] this->matrix;
				this->matrix = nullptr;
			}

			return *this;
		}

		/* Проверка возможности перемножения матриц */
		bool allow_multiply(const Matrix& other) const
		{
			return this->n == other.m;
		}

		/* Проверка возможности сложения матриц */
		bool allow_summ(const Matrix& other) const
		{
			return this->n == other.n && this->m == other.m;
		}

		/* Проверка матрицы на пустоту */
		bool is_empty() const
		{
			return this->matrix == nullptr;
		}

		/* Получить максимальный элемент матрицы */
		T get_max() const
		{
			if (this->is_empty())
			{
				throw "get_max - матрица пустая";
			}

			T max = *this->matrix;

			for (size_t i = 1; i < this->get_size(); i++)
			{
				if (max < *(this->matrix + i))
				{
					max = *(this->matrix + i);
				}
			}

			return max;
		}

		/* Получить минимальный элемент матрицы */
		T get_min() const {
			if (this->is_empty())
			{
				throw "get_min - матрица пустая";
			}

			T min = *this->matrix;
			for (size_t i = 1; i < this->get_size(); i++)
			{
				if (min > *(this->matrix + i))
				{
					min = *(this->matrix + i);
				}
			}

			return min;
		}

		/* Геттеры */
		size_t get_m() const
		{
			return this->m;
		}
		size_t get_n() const
		{
			return this->n;

		}
		size_t get_size() const
		{
			return this->n * this->m;
		}
		size_t get_id() const
		{
			return this->id;
		}

		/* Перегрузка оператора += */
		Matrix& operator+=(const Matrix& other)
		{
			if (!this->allow_summ(other))
			{
				throw "operator+ - матрицы разных размеров";
			}

			for (size_t i = 0; i < this->get_size(); i++)
			{
				*(this->matrix + i) += *(other.matrix + i);
			}

			return *this;
		}

		/* Перегрузка оператора + */
		Matrix operator+(const Matrix& other)
		{
			return move(Matrix(*this) += other);
		}

		/* Перегрузка оператора -= */
		Matrix& operator-=(const Matrix& other)
		{
			if (!this->allow_summ(other))
			{
				throw "operator-= - матрицы разных размеров";
			}

			for (size_t i = 0; i < this->get_size(); i++)
			{
				*(this->matrix + i) -= *(other.matrix + i);
			}

			return *this;
		}

		/* Перегрузка оператора - */
		Matrix operator-(const Matrix& other)
		{
			return move(Matrix(*this) -= other);
		}

		/* Перегрузка оператора *= на другую матрицу */
		Matrix& operator*=(const Matrix& other)
		{
			if (!this->allow_multiply(other))
			{
				throw "operator*= - матрицы разных размеров";
			}

			size_t new_size = this->n * other.m;
			T* matrix = new T[new_size]();

			for (size_t i = 0; i < this->n; i++)
			{
				for (size_t j = 0; j < other.m; j++)
				{
					for (size_t k = 0; k < this->m; k++)
					{
						*(matrix + i * other.m + j) += *(this->matrix + i * this->m + k) * *(other.matrix + k * other.m + j);
					}
				}
			}

			delete[] this->matrix;
			this->matrix = matrix;
			this->m = this->n;
			this->n = other.m;

			return *this;
		}

		/* Перегрузка оператора * */
		Matrix operator*(const Matrix& other)
		{
			return move(Matrix(*this) *= other);
		}

		/* Перегрузка оператора *= на скаляр */
		Matrix& operator*=(T k)
		{
			if (this->is_empty())
			{
				throw "operator* - матрица пустая";
			}

			for (size_t i = 0; i < this->get_size(); i++)
			{
				*(this->matrix + i) *= k;
			}

			return *this;
		}

		/* Перегрузка оператора * на скаляр */
		Matrix operator*(T k)
		{
			return move(Matrix(*this) *= k);
		}

		friend ostream& operator << (ostream& os, Matrix& matrix);

		/* Переопределение оператора вывода на поток << */
		friend ostream& operator << (ostream& os, Matrix& matrix)
		{
			auto width = os.width();
			width = width == 0 ? 8 : width;

			if (matrix.is_empty())
			{
				os << setw(width) << "Матрица пустая" << endl;
				return os;
			}

			for (size_t i = 0; i < matrix.n; i++)
			{
				for (size_t j = 0; j < matrix.m; j++)
				{
					os << setw(width) << *(matrix.matrix + i * matrix.m + j) << " ";
				}
				os << endl;
			}

			return os;
		}

		/* Перегрузка оператора [] */
		Row operator[](size_t x)
		{
			if (x >= this->n)
			{
				throw "Выход за пределы.";
			}
			return Row(this, x);
		}
		const Row operator[](size_t x) const
		{
			if (x >= this->n)
			{
				throw "Выход за пределы.";
			}
			return Row(this, x);
		}
	};

	template <class T>
	bool Matrix<T>::debug = true;
	template <class T>
	size_t Matrix<T>::static_id = 0;
}

#endif

