#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>
#include <future>

#define PICTURESIZE		400
#define STEPSIZE		0.1
#define HOEKSTEPSIZE	0.1

double pX = 0.0;
double pY = 2.0;
double pZ = 0.0;
double rX = 0.0;
double rY = 0.0;
double rZ = 0.0;

bool run = true;

double dx;
double dy;
double dz;
double len;

SDL_Window *window;
SDL_Surface *surface;
SDL_Renderer *renderer;

class Vector {
private:
	double x;
	double y;
	double z;

public:
	Vector() : x(0.0), y(0.0), z(0.0) {}
	Vector(double x, double y, double z) : x(x), y(y), z(z) {}
	~Vector() {}

	void redef(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	friend class IntersectionHandler;

};

class Ray {
private:
	Vector origin;
	Vector direction;

public:
	Ray(double ox, double oy, double oz, double dx, double dy, double dz) : origin(ox, oy, oz), direction(dx, dy, dz) {}
	Ray(Vector const& o, Vector const& d) : origin(o), direction(d) {}
	~Ray() {}

	void redef(double ox, double oy, double oz, double dx, double dy, double dz) {
		origin.redef(ox, oy, oz);
		direction.redef(dx, dy, dz);
	}

	friend class IntersectionHandler;

};

class Sphere {
private:
	Vector origin;
	double radius;

public:
	Sphere(double x, double y, double z, double r) : origin(x, y, z), radius(r) {}
	~Sphere() {}

	friend class IntersectionHandler;

};

Vector p(pX, pY, pZ);
Vector v;
Sphere s(5.0, 0.0, 0.0, 1.0);

class IntersectionHandler {
public:
	static double getIntersection(Ray const& r, Sphere const& s) {
		double a = (r.direction.x * r.direction.x)
		         + (r.direction.y * r.direction.y)
				 + (r.direction.z * r.direction.z);

		double b = 2 * (((r.origin.x - s.origin.x) * r.direction.x)
		         + ((r.origin.y - s.origin.y) * r.direction.y)
		         + ((r.origin.z - s.origin.z) * r.direction.z));

		double c = ((r.origin.x - s.origin.x) * (r.origin.x - s.origin.x))
				 + ((r.origin.y - s.origin.y) * (r.origin.y - s.origin.y))
				 + ((r.origin.z - s.origin.z) * (r.origin.z - s.origin.z))
				 - (s.radius * s.radius);

		double dist = std::max((-b + std::sqrt(b * b - 4 * a * c)) / (2 * a), 
					    	   (-b - std::sqrt(b * b - 4 * a * c)) / (2 * a));

		dist = (dist * r.direction.x * dist * r.direction.x)
		     + (dist * r.direction.y * dist * r.direction.y)
			 + (dist * r.direction.z * dist * r.direction.z);
			 
		dist = std::sqrt(dist);
		return dist;
	}
};

void eventHandler() {
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_Event event;
    const Uint8* keyboardStateArray = SDL_GetKeyboardState(NULL);

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_KEYDOWN:
            if(keyboardStateArray[SDL_SCANCODE_W]) pX += STEPSIZE;
			if(keyboardStateArray[SDL_SCANCODE_A]) pY += STEPSIZE;
			if(keyboardStateArray[SDL_SCANCODE_S]) pX -= STEPSIZE;
			if(keyboardStateArray[SDL_SCANCODE_D]) pY -= STEPSIZE;
			if(keyboardStateArray[SDL_SCANCODE_ESCAPE]) run = false;
        }
    }
}

int main(int argv, char** args) {
	SDL_Init(SDL_INIT_EVERYTHING);
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cerr << "Unable to Init SDL: %s" << SDL_GetError() << std::endl;
    
    window = SDL_CreateWindow("RAYTRACERPROJECT",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              PICTURESIZE, PICTURESIZE, SDL_WINDOW_SHOWN);
	if (window == nullptr) std::cerr << "Unable to create SDL Window: %s" << SDL_GetError() << std::endl;
    
	surface = SDL_GetWindowSurface(window);
    if (surface == nullptr) std::cerr << "Unable to create SDL Surface: %s" << SDL_GetError() << std::endl;
    
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) std::cerr << "Unable to create SDL Renderer: %s" << SDL_GetError() << std::endl;

	while(run) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
        SDL_RenderClear(renderer);

		eventHandler();

		for(int row = -PICTURESIZE / 2; row <= PICTURESIZE / 2; row++) {
			for(int col = -PICTURESIZE / 2; col <= PICTURESIZE / 2; col++) {
				std::future<void> fut = std::async(std::launch::async,
				[row, col] () -> void {
					dx = rX + (double) row / (double) PICTURESIZE;
					dy = rY + (double) col / (double) PICTURESIZE;
					dz = rZ + 1.0;
					len = std::sqrt(dx * dx + dy * dy + dz * dz);

					p.redef(pX, pY, pZ);
					v.redef(dz / len,
							dx / len,
							dy / len);

					Ray r(p, v);
					double dist = IntersectionHandler::getIntersection(r, s);

					if(dist > 0) {
						SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
						SDL_RenderDrawLine(renderer, row + (PICTURESIZE / 2), col + (PICTURESIZE / 2),
													 row + (PICTURESIZE / 2), col + (PICTURESIZE / 2));
					}
				});
			}
		}

		SDL_CreateTexture(renderer,
                          SDL_PIXELFORMAT_RGB332,
                          SDL_TEXTUREACCESS_STREAMING, PICTURESIZE,
                          PICTURESIZE);
        SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
    SDL_Quit();
}

// g++ -Isrc/Include -Lsrc/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -lpthread -lSDL2_image