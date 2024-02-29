#ifndef __HYPERNEAT_NEURALNET_HPP__
#define __HYPERNEAT_NEURALNET_HPP__

#include <HyperNeat/Utils/Size.hpp>
#include <HyperNeat/Utils/ValueMap.hpp>

namespace hyperneat
{
    class Cppn;
    class NeuralNetPrms;

    class NeuralNet
    {
    public:
        class Neuron;

        NeuralNet() = default;

        void create(Cppn& cppn, const NeuralNetPrms& nnPrms);
        void clear();

        size_t getInputsCount() const;
        size_t getOutputsCount() const;
        size_t getNeuronsCount() const;

        const Vector<double*>& getInputs() const;
        const Vector<double*>& getOutputs() const;
        const Vector<Neuron>& getNeurons() const;

        double getAverageActivation() const;

        double& inputAt(size_t i);
        double outputAt(size_t i) const;

        void cycle();

        class Neuron
        {
        public:
            enum class Type {
                INPUT, HIDDEN, OUTPUT
            };

            Neuron() = default;
            Neuron(const Point& position, Type type, double bias);

            void appendInput();
            void flushOutput();

            class Synapse
            {
            public:
                Synapse() = default;
                Synapse(Neuron* inputNeuron, double weight);

                double* _input  = nullptr;
                Neuron* _neuron = nullptr;
                double  _weight = 0.0;
            };

            Vector<Synapse> _synapses;
            Point           _position;
            Type            _type        = Type::HIDDEN;
            double          _bias        = 0.0;
            double          _storedInput = 0.0;
            double          _output      = 0.0;
        };

    private:
        Vector<double*> _inputs;
        Vector<double*> _outputs;
        Vector<Neuron>  _neurons;

        friend class Organism;
    };
}

#endif
