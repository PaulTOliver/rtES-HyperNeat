#ifndef __CPPN_EXPLORER_HPP__
#define __CPPN_EXPLORER_HPP__

#include <Hyperneat/Cppn.hpp>
#include <Hyperneat/Genome.hpp>
#include <Hyperneat/NeuralNet.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <HyperNeat/Utils/Atomic.hpp>
#include <HyperNeat/Utils/Thread.hpp>
#include <Hyperneat/NeuralNetPrms.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace hyperneat
{
    class CppnExplorer
    {
    public:
        CppnExplorer();
        ~CppnExplorer();

        void run(const Genome& genome, const NeuralNetPrms* nnPrms = nullptr, sf::RenderTarget* target = nullptr);
        void shutdown();
        void draw();

        // void run(const Genome& genome, const NeuralNetPrms& nnPrms);
        // void shutdown();

    private:
        void windowHandler();
        void eventHandler();
        void neuralNetGenerator();
        void fieldGenerator();

        class Neuron
        {
        public:
            Neuron(const NeuralNet::Neuron& refNeuron);

            sf::VertexArray _synapses = sf::VertexArray(sf::Lines);
            sf::CircleShape _nucleus  = sf::CircleShape(4.0f * PIXEL_SIZE, 12);
        };

        Cppn           _cppn;
        Genome         _genome;
        NeuralNetPrms  _nnPrms;
        Vector<Neuron> _neurons;
        NeuralNet      _neuralNet;
        bool           _withNeuralNet  = false;
        Atomic<bool>   _neuralNetReady = {false};

        sf::RenderWindow  _window;
        Atomic<bool>      _windowOpen = {false};
        sf::RenderTarget* _target     = nullptr;
        const sf::Color   _uiColor    = {255, 255, 255, 8};

        sf::Image          _fieldImage;
        sf::RectangleShape _fieldSprite;
        sf::Texture        _fieldTexture;
        size_t             _currentField = 0;
        Atomic<bool>       _fieldReady   = {false};

        Thread _windowHandler;
        Thread _fieldGenerator;
        Thread _neuralNetGenerator;

        static constexpr unsigned RESOLUTION = 512;
        static constexpr float    PIXEL_SIZE = 1.0f / static_cast<float>(RESOLUTION);

        // class Neuron
        // {
        // public:
        //     enum class Type {
        //         INPUT,
        //         HIDDEN,
        //         OUTPUT
        //     };

        //     Type            _type;
        //     sf::CircleShape _body     = sf::CircleShape(0.0f, 30);
        //     sf::CircleShape _nucleus  = sf::CircleShape(0.0f, 30);
        //     sf::VertexArray _synapses = sf::VertexArray(sf::Lines);
        // };

        // void print(const sf::Color* field);
        // void placeCursorAt(const sf::Vector2f& newPos);
        // void createNeuron(Neuron& repNeuron, const NeuralNet::Neuron& srcNeuron);

        // Cppn          _cppn;
        // Genome        _genome;
        // NeuralNet     _neuralNet;
        // NeuralNetPrms _nnPrms;

        // const unsigned   RESOLUTION = 512;
        // sf::RenderWindow _window;
        // Atomic<bool>     _running;
        // Thread           _executor;

        // const sf::Color  WEIGHTS      = { 128,   0, 255 };
        // const sf::Color  INV_WEIGHTS  = { 255,   0, 128 };
        // const sf::Color  BIAS         = {   0, 255, 255 };
        // const sf::Color* _field       = &WEIGHTS;

        // sf::Image          _valueImage;
        // sf::Texture        _valueTexture;
        // sf::RectangleShape _valueSprite;
        // sf::Image          _biasImage;
        // sf::Texture        _biasTexture;
        // sf::RectangleShape _biasSprite;
        // bool               _showBias = false;

        // const sf::Color _uiColor = { 64, 64, 64 };
        // const float     PIXEL    = 1.0f / static_cast<float>(RESOLUTION);

        // sf::Vector2f       _cursorPos = { 0.0f, 0.0f };
        // sf::RectangleShape _cursorH   = sf::RectangleShape(sf::Vector2f(0.2f, PIXEL * 3.0f));
        // sf::RectangleShape _cursorV   = sf::RectangleShape(sf::Vector2f(PIXEL * 3.0f, 0.2f));
        // sf::VertexArray    _grid      = sf::VertexArray(sf::Lines);

        // bool           _showField   = true;
        // bool           _showNeurons = true;
        // bool           _showGrid    = true;
        // Vector<Neuron> _neurons;
    };
}

#endif
