#pragma once

#include <string>
#include <list>
#include <vector>
#include <functional>
#include <memory>
#include <cassert>
#include <dlfcn.h>
#include "AbstractAlgorithm.h"
#include <iterator>


enum RegistrationStatus { ALGORITHM_REGISTERED_SUCCESSFULLY = 0, FILE_CANNOT_BE_LOADED = -1, NO_ALGORITHM_REGISTERED = -2 };


class AlgorithmRegistrar {
	//private:
	std::vector<void*> handles;

	std::list<std::string> algorithmNames;

	std::list <std::function<std::unique_ptr<AbstractAlgorithm>()>> algorithmFactories;

	void registerAlgorithm(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithmFactory);

	void setNameForLastAlgorithm(const std::string& algorithmName) {
		assert(algorithmFactories.size() - 1 == algorithmNames.size());
		algorithmNames.push_back(algorithmName);
	}

public:
	friend class AlgorithmRegistration;

	RegistrationStatus loadAlgorithm(const std::string& path, const std::string& so_file_name_without_so_suffix);

	std::list<std::unique_ptr<AbstractAlgorithm>> getAlgorithms() const;

	void getAlgorithmAt(std::unique_ptr<AbstractAlgorithm> &algorithm, int index);

	const std::list<std::string>& getAlgorithmNames() const {
		return algorithmNames;
	}

	size_t size() const {
		return algorithmFactories.size();
	}

	static AlgorithmRegistrar& getInstance() {
		return instance;
	}

	virtual ~AlgorithmRegistrar();

private:
	static AlgorithmRegistrar instance;
};