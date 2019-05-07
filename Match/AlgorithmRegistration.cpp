#include "AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"


//constructor
AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
	AlgorithmRegistrar::getInstance().registerAlgorithm(algorithmFactory);
}
