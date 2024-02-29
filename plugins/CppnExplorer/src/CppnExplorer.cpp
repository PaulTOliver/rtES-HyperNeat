#include <CppnExplorer.hpp>
#include <SFML/Window/Event.hpp>
#include <HyperNeat/Utils/Point.hpp>

using namespace sf;
using namespace hyperneat;

CppnExplorer::CppnExplorer()
{
    _fieldSprite.setOrigin(1.0f, 1.0f);
    _fieldSprite.setPosition(0.0f, 0.0f);
    _fieldSprite.setTexture(&_fieldTexture);
    _fieldSprite.setSize(Vector2f(2.0f, 2.0f));
    _fieldImage.create(RESOLUTION, RESOLUTION);
}

CppnExplorer::~CppnExplorer()
{
    shutdown();
}

void
CppnExplorer::run(const Genome& genome, const NeuralNetPrms* nnPrms, RenderTarget* target)
{
    shutdown();
    _target = target;
    _genome = genome;

    _cppn.clear();
    _cppn.create(_genome);

    if (nnPrms) {
        _withNeuralNet      = true;
        _nnPrms             = *nnPrms;
        _neuralNetGenerator = Thread(&CppnExplorer::neuralNetGenerator, this);
    }

    if (!_target) {
        _windowOpen    = true;
        _target        = &_window;
        _windowHandler = Thread(&CppnExplorer::windowHandler, this);
    }

    _fieldGenerator = Thread(&CppnExplorer::fieldGenerator, this);
}

void
CppnExplorer::shutdown()
{
    if (_neuralNetGenerator.joinable()) {
        _neuralNetGenerator.join();
    }

    if (_windowHandler.joinable()) {
        _windowOpen = false;
        _windowHandler.join();
    }

    _neurons.clear();
    _withNeuralNet  = false;
    _neuralNetReady = false;
    _windowOpen     = false;
    _target         = nullptr;
}

void
CppnExplorer::draw()
{
    _target->clear({8, 8, 16});

    if (_neuralNetReady) {
        for (auto& i : _neurons) {
            _target->draw(i._synapses, BlendAdd);
        }

        for (auto& i : _neurons) {
            _target->draw(i._nucleus, BlendAdd);
        }
    }

    if (_windowOpen) {
        _window.display();
    }
}

void
CppnExplorer::windowHandler()
{
    ContextSettings settings(0, 0, 8);
    String          title("Cppn Explorer");
    View            view({-1.0f, -1.0f, 2.0f, 2.0f});
    VideoMode       videoMode(RESOLUTION, RESOLUTION);
    Uint32          style(Style::Titlebar | Style::Close);

    _window.create(videoMode, title, style, settings);
    _window.setVerticalSyncEnabled(true);
    _window.setView(view);

    while (_windowOpen) {
        eventHandler();
        draw();
    }
}

void
CppnExplorer::eventHandler()
{
    Event event;

    while (_window.pollEvent(event)) {
        switch (event.type) {
            case Event::Closed:
                _windowOpen = false;
                break;

            default:;
        }
    }
}

void
CppnExplorer::neuralNetGenerator()
{
    _neuralNet.clear();
    _neuralNet.create(_cppn, _nnPrms);

    _neurons.reserve(_neuralNet.getNeurons().size());

    for (auto& i : _neuralNet.getNeurons()) {
        _neurons.emplace_back(i);
    }

    _neuralNetReady = true;
}

void
CppnExplorer::fieldGenerator()
{

}

CppnExplorer::Neuron::Neuron(const NeuralNet::Neuron& refNeuron)
{
    _nucleus.setPosition(refNeuron._position._x, refNeuron._position._y);
    _nucleus.setOrigin(_nucleus.getRadius(), _nucleus.getRadius());

    Color nColor = (refNeuron._bias > 0.0 ? Color::White : Color::Red);
    nColor.a     = static_cast<Uint8>(fabs((refNeuron._bias / 3.0) * 112.0) + 16.0);
    _nucleus.setFillColor(nColor);

    switch (refNeuron._type) {
        case NeuralNet::Neuron::Type::INPUT:
            _nucleus.setOutlineThickness(4.0f * PIXEL_SIZE);
            _nucleus.setOutlineColor({255, 255, 255, nColor.a});
            break;

        case NeuralNet::Neuron::Type::OUTPUT:
            _nucleus.setOutlineThickness(4.0f * PIXEL_SIZE);
            _nucleus.setOutlineColor({255, 0, 0, nColor.a});
            break;

        default:;
    }

    for (auto& i : refNeuron._synapses) {
        auto iNeuron  = *i._neuron;
        Color sColor  = (i._weight > 0.0 ? Color::White : Color::Red);
        sColor.a      = static_cast<Uint8>(fabs((i._weight / 3.0) * 112.0) + 16.0);
        Vector2f iPos = {static_cast<float>(iNeuron._position._x), static_cast<float>(iNeuron._position._y)};
        Vector2f tPos = {static_cast<float>(refNeuron._position._x), static_cast<float>(refNeuron._position._y)};

        _synapses.append({iPos, sColor});
        _synapses.append({tPos, sColor});
    }
}

// void
// CppnExplorer::run(const Genome& genome, const NeuralNetPrms& nnPrms)
// {
//     shutdown();

//     _genome = genome;
//     _nnPrms = nnPrms;

//     _cppn.create(_genome);
//     _neuralNet.create(_cppn, _nnPrms);

//     {
//         float gridStep    = 2.0f / 16.0f;
//         Color gridClr = _uiColor;
//         gridClr.a         = 64;

//         for (float i = -1.0f; i <= 1.0f; i += gridStep) {
//             _grid.append({{ -1.0f,     i }, gridClr });
//             _grid.append({{  1.0f,     i }, gridClr });
//             _grid.append({{     i, -1.0f }, gridClr });
//             _grid.append({{     i,  1.0f }, gridClr });
//         }
//     }

//     _executor = Thread([&]() {
//         _running = true;

//         {
//             VideoMode       videoMode(RESOLUTION, RESOLUTION);
//             String          title("Cppn Explorer");
//             Uint32          style(Style::Titlebar | Style::Close);
//             ContextSettings settings(0, 0, 8);
//             View            view(FloatRect(-1.0f, -1.0f, 2.0f, 2.0f));

//             _window.create(videoMode, title, style, settings);
//             _window.setVerticalSyncEnabled(true);
//             _window.setView(view);
//         }

//         _cppn.inputAt(5) = 1.0;

//         _valueImage.create(RESOLUTION, RESOLUTION);
//         _biasImage.create(RESOLUTION, RESOLUTION);
//         print(&WEIGHTS);
//         print(&BIAS);

//         _valueSprite.setTexture(&_valueTexture);
//         _valueSprite.setSize(Vector2f(2.0f, 2.0f));
//         _valueSprite.setOrigin(1.0f, 1.0f);
//         _valueSprite.setPosition(0.0f, 0.0f);

//         _biasSprite.setTexture(&_biasTexture);
//         _biasSprite.setSize(Vector2f(2.0f, 2.0f));
//         _biasSprite.setOrigin(1.0f, 1.0f);
//         _biasSprite.setPosition(0.0f, 0.0f);

//         _cursorH.setFillColor(_uiColor);
//         _cursorV.setFillColor(_uiColor);
//         _cursorH.setOrigin(_cursorH.getSize() / 2.0f);
//         _cursorV.setOrigin(_cursorV.getSize() / 2.0f);
//         _cursorH.setOutlineThickness(PIXEL);
//         _cursorV.setOutlineThickness(PIXEL);
//         _cursorH.setOutlineColor({ 128, 128, 128 });
//         _cursorV.setOutlineColor({ 128, 128, 128 });

//         _neurons.reserve(_neuralNet.getNeuronsCount());

//         for (auto& i : _neuralNet.getNeurons()) {
//             _neurons.emplace_back();
//             createNeuron(_neurons.back(), i);
//         }

//         while (_running) {
//             Event event;

//             while (_window.pollEvent(event)) {
//                 switch (event.type) {
//                     case Event::KeyPressed:
//                         switch (event.key.code) {
//                             case Keyboard::B:
//                                 _showBias = !_showBias;
//                                 break;

//                             case Keyboard::C:
//                                 placeCursorAt({ 0.0f, 0.0f });
//                                 print(_field);
//                                 break;

//                             case Keyboard::F:
//                                 _field == &WEIGHTS ? ++_field : --_field;
//                                 print(_field);
//                                 break;

//                             case Keyboard::V:
//                                 _showField = !_showField;
//                                 break;

//                             case Keyboard::N:
//                                 _showNeurons = !_showNeurons;
//                                 break;

//                             case Keyboard::G:
//                                 _showGrid = !_showGrid;
//                                 break;

//                             default:;
//                         }

//                         break;

//                     case Event::MouseButtonPressed:
//                         if (event.mouseButton.button == Mouse::Left) {
//                             placeCursorAt(_window.mapPixelToCoords(Mouse::getPosition(_window)));
//                             print(_field);
//                         }

//                         break;

//                     case Event::Closed:
//                         _running = false;
//                         break;

//                     default:;
//                 }
//             }

//             _window.clear({ 8, 8, 16 });

//             if (_showField) {
//                 if (_showBias) {
//                     _window.draw(_biasSprite);
//                 } else {
//                     _window.draw(_valueSprite);
//                 }
//             }

//             if (_showGrid) {
//                 _window.draw(_grid, { BlendMode::BlendAdd });
//             }

//             if (_showNeurons) {
//                 for (auto& i : _neurons) {
//                     _window.draw(i._synapses);
//                 }

//                 for (auto& i : _neurons) {
//                     if (i._type != Neuron::Type::HIDDEN) {
//                         continue;
//                     }

//                     _window.draw(i._body);
//                     _window.draw(i._nucleus);
//                 }

//                 for (auto& i : _neurons) {
//                     if (i._type == Neuron::Type::HIDDEN) {
//                         continue;
//                     }

//                     _window.draw(i._body);
//                     _window.draw(i._nucleus);
//                 }
//             }

//             _window.draw(_cursorH);
//             _window.draw(_cursorV);

//             _window.display();
//         }
//     });
// }

// void
// CppnExplorer::shutdown()
// {
//     if (_executor.joinable()) {
//         _running = false;
//         _executor.join();
//     }

//     this->~CppnExplorer();
//     new (this) CppnExplorer();
// }

// void
// CppnExplorer::print(const Color* field)
// {
//     size_t       out     = 0;
//     double*      x1      = nullptr;
//     double*      y1      = nullptr;
//     double*      x2      = nullptr;
//     double*      y2      = nullptr;
//     Image*   image   = nullptr;
//     Texture* texture = nullptr;

//     if (field == &WEIGHTS) {
//         x1 = &_cppn.inputAt(0);
//         y1 = &_cppn.inputAt(1);
//         x2 = &_cppn.inputAt(2);
//         y2 = &_cppn.inputAt(3);
//     } else {
//         x1 = &_cppn.inputAt(2);
//         y1 = &_cppn.inputAt(3);
//         x2 = &_cppn.inputAt(0);
//         y2 = &_cppn.inputAt(1);
//     }

//     *x1 = _cursorPos.x;
//     *y1 = _cursorPos.y;

//     if (field == &BIAS) {
//         out     = 1;
//         image   = &_biasImage;
//         texture = &_biasTexture;
//     } else {
//         image   = &_valueImage;
//         texture = &_valueTexture;
//     }

//     double half = static_cast<double>(RESOLUTION / 2);

//     for (unsigned y = 0; y < RESOLUTION; ++y) {
//         for (unsigned x = 0; x < RESOLUTION; ++x) {
//             *x2 = (static_cast<double>(x) - half) / half;
//             *y2 = (static_cast<double>(y) - half) / half;
//             _cppn.inputAt(4) = Point(*x1, *y1).distance(Point(*x2, *y2));
//             _cppn.cycle();

//             auto multColor = [](const Color& base, double factor) {
//                 Color result;

//                 result.r = static_cast<Uint8>(static_cast<double>(base.r) * factor);
//                 result.g = static_cast<Uint8>(static_cast<double>(base.g) * factor);
//                 result.b = static_cast<Uint8>(static_cast<double>(base.b) * factor);

//                 return result;
//             };

//             if (_cppn.outputAt(out) > 0) {
//                  image->setPixel(x, y, multColor(Color::White, _cppn.outputAt(out)));
//             } else {
//                  image->setPixel(x, y, multColor(*field, fabs(_cppn.outputAt(out))));
//             }
//         }
//     }

//     texture->loadFromImage(*image);
// }

// void
// CppnExplorer::placeCursorAt(const Vector2f& newPos)
// {
//     _cursorPos = newPos;
//     _cursorH.setPosition(_cursorPos);
//     _cursorV.setPosition(_cursorPos);
// }

// void
// CppnExplorer::createNeuron(Neuron& repNeuron, const NeuralNet::Neuron& srcNeuron)
// {
//     repNeuron._body.setOutlineThickness(PIXEL * 2.0f);
//     repNeuron._nucleus.setOutlineThickness(PIXEL * 2.0f);

//     float     radius      = 0.0f;
//     Color inputColor  = BIAS;
//     Color hiddenColor = INV_WEIGHTS;
//     Color outputColor = WEIGHTS;

//     inputColor.a  = 128;
//     hiddenColor.a = 128;
//     outputColor.a = 128;

//     switch (srcNeuron._type) {
//         case NeuralNet::Neuron::Type::INPUT:
//             repNeuron._type = Neuron::Type::INPUT;
//             repNeuron._body.setOutlineColor(inputColor);
//             repNeuron._body.setFillColor({ 0, 0, 0, 200 });
//             radius = PIXEL * 24.0f;
//             break;

//         case NeuralNet::Neuron::Type::HIDDEN:
//             repNeuron._type = Neuron::Type::HIDDEN;
//             repNeuron._body.setOutlineColor(hiddenColor);
//             repNeuron._body.setFillColor({ 0, 0, 0, 200 });

//             {
//                 float level = 1.0f;
//                 for (; srcNeuron._position._x * level != floor(srcNeuron._position._x * level); level *= 2.0f);
//                 radius = 1.0f / (level * 4.0f);

//                 if (radius > PIXEL * 16.0f) {
//                     radius = PIXEL * 16.0f;
//                 }
//             }

//             break;

//         case NeuralNet::Neuron::Type::OUTPUT:
//             repNeuron._type = Neuron::Type::OUTPUT;
//             repNeuron._body.setOutlineColor(outputColor);
//             repNeuron._body.setFillColor({ 0, 0, 0, 200 });
//             radius = PIXEL * 24.0f;
//             break;
//     }

//     repNeuron._body.setRadius(radius);
//     repNeuron._body.setOrigin(radius, radius);
//     repNeuron._body.setPosition(srcNeuron._position._x, srcNeuron._position._y);

//     repNeuron._nucleus.setRadius(radius / 2.0f);
//     repNeuron._nucleus.setOrigin(radius / 2.0f, radius / 2.0f);
//     repNeuron._nucleus.setPosition(srcNeuron._position._x, srcNeuron._position._y);
//     repNeuron._nucleus.setOutlineColor(repNeuron._body.getOutlineColor());

//     double    multInt = static_cast<Uint8>(fabs(srcNeuron._bias / 3.0) * 255.0);
//     Color colorMult(multInt, multInt, multInt, 128);

//     if (srcNeuron._bias > 0) {
//         repNeuron._nucleus.setFillColor(Color::White * colorMult);
//     } else {
//         repNeuron._nucleus.setFillColor(BIAS * colorMult);
//     }

//     Color synColor = WEIGHTS;

//     for (auto& i : srcNeuron._synapses) {
//         for (auto& j : _neuralNet.getNeurons()) {
//             if (&j._output == i._input) {
//                 Uint8    weight = static_cast<Uint8>(i._weight * 20.0);
//                 Vector2f srcPos(j._position._x, j._position._y);

//                 if (i._input == &srcNeuron._output) {
//                     Color selfSynColor;

//                     if (i._weight > 0.0) {
//                         selfSynColor = Color(255, 255, 255, 20 + weight);
//                     } else {
//                         selfSynColor = synColor;
//                     }

//                     float        rad2 = repNeuron._body.getRadius() * 2.0f;
//                     Vector2f p1   = repNeuron._body.getPosition() + Vector2f(-rad2, -rad2);
//                     Vector2f p2   = repNeuron._body.getPosition() + Vector2f(-rad2,  rad2);
//                     Vector2f p3   = repNeuron._body.getPosition() + Vector2f( rad2,  rad2);
//                     Vector2f p4   = repNeuron._body.getPosition() + Vector2f( rad2, -rad2);

//                     repNeuron._synapses.append({ p1, selfSynColor });
//                     repNeuron._synapses.append({ p2, selfSynColor });

//                     repNeuron._synapses.append({ p2, selfSynColor });
//                     repNeuron._synapses.append({ p3, selfSynColor });

//                     repNeuron._synapses.append({ p3, selfSynColor });
//                     repNeuron._synapses.append({ p4, selfSynColor });

//                     repNeuron._synapses.append({ p4, selfSynColor });
//                     repNeuron._synapses.append({ p1, selfSynColor });
//                 } else {
//                     repNeuron._synapses.append({ repNeuron._body.getPosition(), Color::Black });

//                     if (i._weight > 0.0) {
//                         repNeuron._synapses.append({{ srcPos }, Color(255, 255, 255, 20 + weight) });
//                     } else {
//                         synColor.a = 20 + weight;
//                         repNeuron._synapses.append({{ srcPos }, synColor });
//                     }
//                 }

//                 break;
//             }
//         }
//     }
// }
