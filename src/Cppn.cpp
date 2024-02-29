#include <cmath>
#include <HyperNeat/Cppn.hpp>
#include <HyperNeat/Genome.hpp>
#include <HyperNeat/QuadTree.hpp>
#include <HyperNeat/Utils/Pi.hpp>
#include <HyperNeat/NodeSearchPrms.hpp>

using namespace std;
using namespace hyperneat;

void
Cppn::create(const Genome& genome)
{
    using DepthSorter = Map<double, Map<size_t, const Genome::NodeGene*>>;
    DepthSorter dSorter;

    for (auto& i : genome._nodeGenes) {
        dSorter[i.second._depth][i.first] = &i.second;
    }

    _inputs.resize(genome._inputs, 0.0);
    _nodeLayers.resize(dSorter.size());

    {
        auto layerIter = _nodeLayers.begin();

        for (auto& i : dSorter) {
            layerIter->resize(i.second.size());
            auto nodeIter = layerIter->begin();

            for (auto& j : i.second) {
                size_t enabledLinks = 0;

                for (auto& k : j.second->_linkGenes) {
                    if (k.second._isEnabled) {
                        ++enabledLinks;
                    }
                }

                nodeIter->_links.resize(enabledLinks);
                ++nodeIter;
            }

            ++layerIter;
        }
    }

    size_t layerIdx = 0;

    for (auto& i : dSorter) {
        size_t nodeIdx = 0;

        for (auto& j : i.second) {
            Node& crntNode = _nodeLayers[layerIdx][nodeIdx];
            size_t linkIdx = 0;

            for (auto& k : j.second->_linkGenes) {
                if (k.second._isEnabled) {
                    Node::Link& crntLink = crntNode._links[linkIdx];

                    if (k.first < _inputs.size()) {
                        crntLink._input = &_inputs[k.first];
                    } else {
                        double depth = genome._nodeGenes.at(k.first)._depth;
                        size_t layer = distance(dSorter.begin(), dSorter.find(depth));
                        size_t node  = distance(dSorter[depth].begin(), dSorter[depth].find(k.first));

                        crntLink._input = &_nodeLayers[layer][node]._output;
                    }

                    crntLink._weight = k.second._weight;
                    ++linkIdx;
                }
            }

            crntNode._nodeType = j.second->_nodeType;
            ++nodeIdx;

            if (i.first == 1.0) {
                _outputs.push_back(&crntNode._output);
            }
        }

        ++layerIdx;
    }
}

void
Cppn::clear()
{
    _inputs.clear();
    _outputs.clear();
    _nodeLayers.clear();
}

size_t
Cppn::getInputsCount() const
{
    return _inputs.size();
}

size_t
Cppn::getOutputsCount() const
{
    return _outputs.size();
}

size_t
Cppn::getNodesCount() const
{
    size_t nodesCount = 0;

    for (auto& i : _nodeLayers) {
        nodesCount += i.size();
    }

    return nodesCount;
}

double&
Cppn::inputAt(size_t i)
{
    return _inputs[i];
}

double
Cppn::outputAt(size_t i) const
{
    return *_outputs[i];
}

void
Cppn::cycle()
{
    for (auto& i : _nodeLayers) {
        for (auto& j : i) {
            j.appendInput();
        }

        for (auto& j : i) {
            j.flushOutput();
        }
    }
}

void
Cppn::findNodesIn2DSection(ValueMap& valueMap, const NodeSearchPrms& nsPrms, const Point& source)
{
    QuadTree qTree(1.0, 0.0, 0.0);

    qTree.subdivide([&](QuadTree* qt) {
        auto levelToSegment = [](size_t level) {
            double dlevel = static_cast<double>(level);
            return 1.0 / pow(2.0, dlevel);
        };

        double minQuadTreeSegment = levelToSegment(nsPrms._maxQuadTreeLevel);
        double maxQuadTreeSegment = levelToSegment(nsPrms._minQuadTreeLevel);
        double testGridResolution = levelToSegment(nsPrms._testGridLevel);

        if (qt->getSegment() <= minQuadTreeSegment) {
            return false;
        }

        if (qt->getSegment() > maxQuadTreeSegment) {
            return true;
        }

        double gridStep = qt->getSegment() * 2.0 * testGridResolution;
        double gridHalf = gridStep / 2.0;
        double initX    = qt->getX() - qt->getSegment() + gridHalf;
        double initY    = qt->getY() - qt->getSegment() + gridHalf;
        double endX     = qt->getX() + qt->getSegment();
        double endY     = qt->getY() + qt->getSegment();
        double cellsNo  = (1.0 / testGridResolution) * (1.0 / testGridResolution);

        Vector<double> w;
        w.reserve(static_cast<size_t>(cellsNo));

        for (double y = initY; y < endY; y += gridStep) {
            for (double x = initX; x < endX; x += gridStep) {
                _inputs[nsPrms._x] = x;
                _inputs[nsPrms._y] = y;

                if (nsPrms._useDistance) {
                    _inputs[nsPrms._d] = Point(x, y).distance(source);
                }

                cycle();
                w.emplace_back(*_outputs[nsPrms._o]);
            }
        }

        double wMean    = 0.0;
        double variance = 0.0;

        for (auto& i : w) {
            wMean += i;
        }

        wMean /= cellsNo;

        for (auto& i : w) {
            variance += (wMean - i) * (wMean - i);
        }

        variance /= cellsNo;

        if (variance > nsPrms._varianceThreshold) {
            return true;
        }

        return false;
    });

    qTree.traverse([&](const QuadTree* qt) {
        auto measureDistAndIO = [&]() {
            if (nsPrms._useDistance) {
                _inputs[nsPrms._d] = Point(_inputs[nsPrms._x], _inputs[nsPrms._y]).distance(source);
            }

            cycle();
            return *_outputs[nsPrms._o];
        };

        double gridSize  = qt->getSegment() * 2.0;

        _inputs[nsPrms._y] = qt->getY();
        _inputs[nsPrms._x] = qt->getX() - gridSize;
        double valLeft     = measureDistAndIO();

        _inputs[nsPrms._x] = qt->getX() + gridSize;
        double valRight    = measureDistAndIO();

        _inputs[nsPrms._x] = qt->getX();
        _inputs[nsPrms._y] = qt->getY() - gridSize;
        double valBottom   = measureDistAndIO();

        _inputs[nsPrms._y] = qt->getY() + gridSize;
        double valTop      = measureDistAndIO();

        _inputs[nsPrms._y] = qt->getY();
        double valCenter   = measureDistAndIO();

        valLeft   = fabs(valCenter - valLeft);
        valRight  = fabs(valCenter - valRight);
        valBottom = fabs(valCenter - valBottom);
        valTop    = fabs(valCenter - valTop);

        double bandValue = max(min(valLeft, valRight), min(valBottom, valTop));

        if (bandValue > nsPrms._bandPruningThreshold) {
            valueMap.emplace_back(qt->getX(), qt->getY(), valCenter, qt->getSegment());
        }
    });
}

void
Cppn::Node::appendInput()
{
    for (auto& i : _links) {
        _storedInput += *i._input * i._weight;
    }
}

void
Cppn::Node::flushOutput()
{
    switch (_nodeType) {
        case NodeType::SIGMOID:
            _output = 1.0 / (1.0 + exp(-_storedInput * 2.0));
            _output = _output * 2.0 - 1.0;
            break;

        case NodeType::GAUSSIAN:
            _output = exp(-_storedInput * _storedInput);
            _output = _output * 2.0 - 1.0;
            break;

        case NodeType::SINE:
            _output = sin((_storedInput / 2.0) * PI);
            break;

        case NodeType::ABSOLUTE:
            _output = max(min(fabs(_storedInput), 0.0), 1.0);
            break;

        default:;
    }

    _storedInput = 0.0;
}
