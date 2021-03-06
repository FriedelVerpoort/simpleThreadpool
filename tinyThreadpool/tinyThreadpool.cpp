#include "stdafx.h"
#include "Threadpool.h"
#include <iostream>
#include <complex>
#include <future>
using namespace simpleThreadpool;

//some heavy calculating function
void compute_mandelbrot(double left, double right, double top, double bottom)
{
	const int WIDTH = 300;
	const int HEIGHT = 300;
	uint32_t image[HEIGHT][WIDTH];

	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			std::complex<double> c(left + (x * (right - left) / WIDTH),
				top + (y * (bottom - top) / HEIGHT));

			std::complex<double> z(0.0, 0.0);
			int iterations = 0;
			while (abs(z) < 2.0 && iterations < 500)
			{
				z = (z * z) + c;

				++iterations;
			}

			if (iterations == 500)
				image[y][x] = 0x000000;
			else
				image[y][x] = 0xFFFFFF;
		}
	}
}

void testReturn(std::promise<int>& promise)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(425));
	promise.set_value(5);
}

int main()
{
	srand(unsigned int(time(nullptr)));
	Threadpool m_threadpool(simpleThreadpool::Threadpool::GetMaxThreads());

	std::promise<int> promise{};
	m_threadpool.AddJob(new simpleThreadpool::ThreadFunction<std::promise<int>&>(testReturn, std::ref(promise)));
	int i = promise.get_future().get();
	std::cout << i << '\n';

	m_threadpool.AddJobs(std::initializer_list<simpleThreadpool::IThreadFunction*>{
		new ThreadFunction<double, double, double, double>(compute_mandelbrot, -2.0, 1.0, 1.125, -1.125),
			new ThreadFunction<double, double, double, double>(compute_mandelbrot, -2.0, 1.0, 1.125, -1.125),
			new ThreadFunction<double, double, double, double>(compute_mandelbrot, -2.0, 1.0, 1.125, -1.125),
			new ThreadFunction<double, double, double, double>(compute_mandelbrot, -2.0, 1.0, 1.125, -1.125),
			new ThreadFunction<double, double, double, double>(compute_mandelbrot, -2.0, 1.0, 1.125, -1.125),
			new ThreadFunction<double, double, double, double>(compute_mandelbrot, -2.0, 1.0, 1.125, -1.125),
			new ThreadFunction<double, double, double, double>(compute_mandelbrot, -2.0, 1.0, 1.125, -1.125),
			new ThreadFunction<double, double, double, double>(compute_mandelbrot, -2.0, 1.0, 1.125, -1.125),
			new ThreadFunction<double, double, double, double>(compute_mandelbrot, -2.0, 1.0, 1.125, -1.125),
			new ThreadFunction<double, double, double, double>(compute_mandelbrot, -2.0, 1.0, 1.125, -1.125),
			new ThreadFunction<double, double, double, double>(compute_mandelbrot,-2.0, 1.0, 1.125, -1.125)
	});
	
    return 0;
}