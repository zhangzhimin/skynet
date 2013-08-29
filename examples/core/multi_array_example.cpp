#include <skynet/core/array>

using namespace skynet;

int main(){
	multi_array<int, 2> mat(10,10);
	for (size_t i = 0; i < mat.size(); ++i){
		mat[i] = 1;
	}

	

	return 0;
}
