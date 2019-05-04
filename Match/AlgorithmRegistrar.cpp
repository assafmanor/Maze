#include "AlgorithmRegistrar.h"


/**
 * Adds a new algorithm factory to the list of algorithm factories.
 */
void AlgorithmRegistrar::registerAlgorithm(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
	instance.algorithmFactories.push_back(algorithmFactory);
}


/**
 *
 */
RegistrationStatus AlgorithmRegistrar::loadAlgorithm(const std::string& path, const std::string& so_file_name_without_so_suffix) {
	size_t size = instance.size();
	void *handle;

	//try to load the .so file
	handle = dlopen(path.c_str(),  RTLD_LAZY);
	if(!handle) {
		return FILE_CANNOT_BE_LOADED;
	}
	//after opening the .so file, registerAlgorithm should be called from inside the .so.
	//if for some reason it wasn't called, the instance size should remain the same.
	if (instance.size() == size) {
		return NO_ALGORITHM_REGISTERED;
	}
	instance.setNameForLastAlgorithm(so_file_name_without_so_suffix);
	return ALGORITHM_REGISTERED_SUCCESSFULLY;
}


/**
 *
 */
std::list<std::unique_ptr<AbstractAlgorithm>> AlgorithmRegistrar::getAlgorithms() const {
	std::list<std::unique_ptr<AbstractAlgorithm>> algorithms;
	//activate all of the algorithm factory functions (in order to create the algorithms) and push them back into the algorithms list.
	for (auto algorithmFactoryFunc : algorithmFactories) {
		algorithms.push_back(algorithmFactoryFunc());
	}
	return algorithms;
}

//create the static instance field
AlgorithmRegistrar AlgorithmRegistrar::instance;