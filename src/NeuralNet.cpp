#include <cmath>
#include <algorithm>
#include <HyperNeat/Cppn.hpp>
#include <HyperNeat/NeuralNet.hpp>
#include <HyperNeat/Utils/Map.hpp>
#include <HyperNeat/Utils/Set.hpp>
#include <HyperNeat/NeuralNetPrms.hpp>
#include <HyperNeat/Utils/Function.hpp>
#include <HyperNeat/NodeSearchPrms.hpp>

using namespace std;
using namespace hyperneat;

void
NeuralNet::create(Cppn& cppn, const NeuralNetPrms& nnPrms)
{
    class TempNeuronSynapse {
    public:
        TempNeuronSynapse() = default;
        TempNeuronSynapse(double weight, Point neuronSource)
            : _weight(weight), _neuronSource(neuronSource)
        {}

        double _weight = 0.0;
        Point  _neuronSource;
    };

    class TempNeuron {
    public:
        bool                      _isIncluded = false;
        double                    _bias       = 0.0;
        Vector<TempNeuronSynapse> _neuronSynapses;
    };

    _inputs.reserve(nnPrms._inputMap.size());
    _outputs.reserve(nnPrms._outputMap.size());

    NodeSearchPrms nsPrms(0, 2, 3, 4);
    nsPrms.importFrom(nnPrms);
    cppn.inputAt(5) = 1.0;

    Map<Point, TempNeuron> tempNeurons;

    auto findConnections = [&](const Point& source, size_t x1, size_t y1, size_t x2, size_t y2, size_t d,
            bool checkExist, Function<void(double, const Point&)> storeConn) {
        cppn.inputAt(x1) = source._x;
        cppn.inputAt(y1) = source._y;
        cppn.inputAt(x2) = 0.0;
        cppn.inputAt(y2) = 0.0;
        cppn.inputAt(d)  = 0.0;

        ValueMap newConnections;
        cppn.findNodesIn2DSection(newConnections, nsPrms, source);

        for (auto& i : newConnections) {
            if (checkExist && !tempNeurons.count(i)) {
                continue;
            }

            if (fabs(i._value) > 0.2) {
                storeConn((i._value + (i._value > 0 ? -0.2 : 0.2)) * 3.75, i);
            }
        }
    };

    {
        Set<Point>  neuronSet1;
        Set<Point>  neuronSet2;
        Set<Point>* previousNeurons = &neuronSet1;
        Set<Point>* nextNeurons     = &neuronSet2;

        for (auto& i : nnPrms._inputMap) {
            tempNeurons[i];
            previousNeurons->insert(i);
        }

        for (size_t i = 0; i < nnPrms._iterations && !previousNeurons->empty(); ++i) {
            Map<Point, TempNeuron> newNeurons;

            for (auto& j : *previousNeurons) {
                findConnections(j, 0, 1, 2, 3, 4, false, [&](double weight, const Point& target) {
                    if (tempNeurons.count(target)) {
                        auto& synapses = tempNeurons[target]._neuronSynapses;
                        synapses.emplace_back(weight, j);
                    } else {
                        auto& synapses = newNeurons[target]._neuronSynapses;
                        synapses.emplace_back(weight, j);
                        nextNeurons->insert(target);
                    }
                });
            }

            previousNeurons->clear();
            swap(nextNeurons, previousNeurons);
            tempNeurons.insert(newNeurons.begin(), newNeurons.end());
        }
    }

    nsPrms._x = 0;
    nsPrms._y = 1;

    {
        Vector<TempNeuron*>  inclusionSet1;
        Vector<TempNeuron*>  inclusionSet2;
        Vector<TempNeuron*>* crntInclusions = &inclusionSet1;
        Vector<TempNeuron*>* nextInclusions = &inclusionSet2;

        for (auto& i : nnPrms._outputMap) {
            tempNeurons[i]._isIncluded = true;
            nextInclusions->push_back(&tempNeurons[i]);

            findConnections(i, 2, 3, 0, 1, 4, true, [&](double weight, const Point& target) {
                auto& synapses = tempNeurons[i]._neuronSynapses;
                synapses.emplace_back(weight, target);
            });
        }

        while (!nextInclusions->empty()) {
            crntInclusions->clear();
            swap(crntInclusions, nextInclusions);

            for (auto& i : *crntInclusions) {
                for (auto& j : i->_neuronSynapses) {
                    auto& sourceNeuron = tempNeurons.at(j._neuronSource);

                    if (!sourceNeuron._isIncluded) {
                        nextInclusions->push_back(&sourceNeuron);
                        sourceNeuron._isIncluded = true;
                    }
                }
            }
        }
    }

    for (auto& i : nnPrms._inputMap) {
        tempNeurons[i]._isIncluded = true;
    }

    cppn.inputAt(2) = 0.0;
    cppn.inputAt(3) = 0.0;
    cppn.inputAt(4) = 0.0;

    for (auto i = tempNeurons.begin(), end = tempNeurons.end(); i != end;) {
        if (i->second._isIncluded) {
            cppn.inputAt(0) = i->first._x;
            cppn.inputAt(1) = i->first._y;
            cppn.inputAt(4) = i->first.distance(Point());
            cppn.cycle();
            i->second._bias = cppn.outputAt(1) * 3.0;
            ++i;
        } else {
            i = tempNeurons.erase(i);
        }
    }

    _neurons.resize(tempNeurons.size());

    {
        auto nIter = _neurons.begin();

        for (auto& i : tempNeurons) {
            nIter->_bias         = i.second._bias;
            nIter->_position     = i.first;
            auto& crntNrnSyns    = nIter->_synapses;
            auto& neuronSynapses = i.second._neuronSynapses;
            crntNrnSyns.reserve(neuronSynapses.size());

            for (auto& j : neuronSynapses) {
                auto src    = tempNeurons.find(j._neuronSource);
                size_t sIdx = distance(tempNeurons.begin(), src);
                crntNrnSyns.emplace_back(&_neurons[sIdx], j._weight);
            }

            ++nIter;
        }
    }

    auto relateIO = [&](Vector<double*>& ptrVec, const Vector<Point>& map, Neuron::Type type) {
        for (auto& i : map) {
            auto neuron          = tempNeurons.find(i);
            size_t nIdx          = distance(tempNeurons.begin(), neuron);
            _neurons[nIdx]._type = type;
            ptrVec.push_back(&_neurons[nIdx]._output);
        }
    };

    relateIO(_inputs, nnPrms._inputMap, Neuron::Type::INPUT);
    relateIO(_outputs, nnPrms._outputMap, Neuron::Type::OUTPUT);
}

void
NeuralNet::clear()
{
    _inputs.clear();
    _outputs.clear();
    _neurons.clear();
}

size_t
NeuralNet::getInputsCount() const
{
    return _inputs.size();
}

size_t
NeuralNet::getOutputsCount() const
{
    return _outputs.size();
}

size_t
NeuralNet::getNeuronsCount() const
{
    return _neurons.size();
}

double
NeuralNet::getAverageActivation() const
{
    double totalActivation = 0.0;

    for (auto& i : _neurons) {
        totalActivation += i._output;
    }

    return totalActivation / static_cast<double>(_neurons.size());
}

double&
NeuralNet::inputAt(size_t i)
{
    return *_inputs[i];
}

double
NeuralNet::outputAt(size_t i) const
{
    return *_outputs[i];
}

const Vector<double*>&
NeuralNet::getInputs() const
{
    return _inputs;
}

const Vector<double*>&
NeuralNet::getOutputs() const
{
    return _outputs;
}

const Vector<NeuralNet::Neuron>&
NeuralNet::getNeurons() const
{
    return _neurons;
}

void
NeuralNet::cycle()
{
    for (auto& i : _neurons) {
        i.appendInput();
    }

    for (auto& i : _neurons) {
        i.flushOutput();
    }
}

NeuralNet::Neuron::Neuron(const Point& position, Type type, double bias)
    : _position(position), _type(type), _bias(bias)
{}

void
NeuralNet::Neuron::appendInput()
{
    for (auto& i : _synapses) {
        _storedInput += *i._input * i._weight;
    }

    _storedInput += _bias;
}

void
NeuralNet::Neuron::flushOutput()
{
    _output      = 1.0 / (1.0 + exp(-_storedInput * 4.9));
    _storedInput = 0.0;
}

NeuralNet::Neuron::Synapse::Synapse(Neuron* inputNeuron, double weight)
    : _input(&inputNeuron->_output), _neuron(inputNeuron), _weight(weight)
{}
