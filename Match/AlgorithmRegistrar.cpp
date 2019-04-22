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
	/*TEMP*/bool someBool = true;

	//TODO: load the .so file
	if (someBool/*something*/) {
		//do something
	}
	else {
		return FILE_CANNOT_BE_LOADED;
	}
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