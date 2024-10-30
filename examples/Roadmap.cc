/*
 * Roadmap.cc
 * RVO2 Library
 *
 * SPDX-FileCopyrightText: 2008 University of North Carolina at Chapel Hill
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Please send all bug reports to <geom@cs.unc.edu>.
 *
 * The authors may be contacted via:
 *
 * Jur van den Berg, Stephen J. Guy, Jamie Snape, Ming C. Lin, Dinesh Manocha
 * Dept. of Computer Science
 * 201 S. Columbia St.
 * Frederick P. Brooks, Jr. Computer Science Bldg.
 * Chapel Hill, N.C. 27599-3175
 * United States of America
 *
 * <https://gamma.cs.unc.edu/RVO2/>
 */

/*
 * @file  Roadmap.cc
 * @brief Example file showing a demo with 100 agents split in four groups
 *        initially positioned in four corners of the environment. Each agent
 *        attempts to move to other side of the environment through a narrow
 *        passage generated by four obstacles. There is a roadmap to guide the
 *        agents around the obstacles.
 */

#ifndef RVO_OUTPUT_TIME_AND_POSITIONS
#define RVO_OUTPUT_TIME_AND_POSITIONS 1
#endif /* RVO_OUTPUT_TIME_AND_POSITIONS */

#ifndef RVO_SEED_RANDOM_NUMBER_GENERATOR
#define RVO_SEED_RANDOM_NUMBER_GENERATOR 1
#endif /* RVO_SEED_RANDOM_NUMBER_GENERATOR */

#include <cmath>
#include <cstdlib>
#include <limits>
#include <map>
#include <utility>
#include <vector>

#include "RVOSimulator.h"
#include "Vector2.h"
#include "raylib.h"

#if RVO_OUTPUT_TIME_AND_POSITIONS
#include <iostream>
#endif /* RVO_OUTPUT_TIME_AND_POSITIONS */

#if RVO_SEED_RANDOM_NUMBER_GENERATOR
#include <ctime>
#endif /* RVO_SEED_RANDOM_NUMBER_GENERATOR */

#if _OPENMP
#include <omp.h>
#endif /* _OPENMP */

namespace {
const float RVO_TWO_PI = 6.28318530717958647692F;

class RoadmapVertex {
public:
  RVO::Vector2 position;
  std::vector<int> neighbors;
  std::vector<float> distToGoal;
};

void setupScenario(
    RVO::RVOSimulator *simulator,
    std::vector<RoadmapVertex> &roadmap, /* NOLINT(runtime/references) */
    std::vector<int> &goals) {           /* NOLINT(runtime/references) */
#if RVO_SEED_RANDOM_NUMBER_GENERATOR
  std::srand(static_cast<unsigned int>(std::time(NULL)));
#endif /* RVO_SEED_RANDOM_NUMBER_GENERATOR */

  /* Specify the global time step of the simulation. */
  simulator->setTimeStep(0.25F);

  /* Add polygonal obstacles, specifying their vertices in counterclockwise
   * order.*/
  std::vector<RVO::Vector2> obstacle1;
  std::vector<RVO::Vector2> obstacle2;
  std::vector<RVO::Vector2> obstacle3;
  std::vector<RVO::Vector2> obstacle4;

  obstacle1.push_back(RVO::Vector2(-10.0F, 40.0F));
  obstacle1.push_back(RVO::Vector2(-40.0F, 40.0F));
  obstacle1.push_back(RVO::Vector2(-40.0F, 10.0F));
  obstacle1.push_back(RVO::Vector2(-10.0F, 10.0F));

  obstacle2.push_back(RVO::Vector2(10.0F, 40.0F));
  obstacle2.push_back(RVO::Vector2(10.0F, 10.0F));
  obstacle2.push_back(RVO::Vector2(40.0F, 10.0F));
  obstacle2.push_back(RVO::Vector2(40.0F, 40.0F));

  obstacle3.push_back(RVO::Vector2(10.0F, -40.0F));
  obstacle3.push_back(RVO::Vector2(40.0F, -40.0F));
  obstacle3.push_back(RVO::Vector2(40.0F, -10.0F));
  obstacle3.push_back(RVO::Vector2(10.0F, -10.0F));

  obstacle4.push_back(RVO::Vector2(-10.0F, -40.0F));
  obstacle4.push_back(RVO::Vector2(-10.0F, -10.0F));
  obstacle4.push_back(RVO::Vector2(-40.0F, -10.0F));
  obstacle4.push_back(RVO::Vector2(-40.0F, -40.0F));

  simulator->addObstacle(obstacle1);
  simulator->addObstacle(obstacle2);
  simulator->addObstacle(obstacle3);
  simulator->addObstacle(obstacle4);

  /* Process the obstacles so that they are accounted for in the simulation. */
  simulator->processObstacles();

  /* Add roadmap vertices. */
  RoadmapVertex v;

  /* Add the goal positions of agents. */
  v.position = RVO::Vector2(-75.0F, -75.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(75.0F, -75.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(-75.0F, 75.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(75.0F, 75.0F);
  roadmap.push_back(v);

  /* Add roadmap vertices around the obstacles. */
  v.position = RVO::Vector2(-42.0F, -42.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(-42.0F, -8.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(-42.0F, 8.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(-42.0F, 42.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(-8.0F, -42.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(-8.0F, -8.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(-8.0F, 8.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(-8.0F, 42.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(8.0F, -42.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(8.0F, -8.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(8.0F, 8.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(8.0F, 42.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(42.0F, -42.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(42.0F, -8.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(42.0F, 8.0F);
  roadmap.push_back(v);
  v.position = RVO::Vector2(42.0F, 42.0F);
  roadmap.push_back(v);

  /* Specify the default parameters for agents that are subsequently added. */
  simulator->setAgentDefaults(15.0F, 10U, 5.0F, 5.0F, 2.0F, 2.0F);

  /* Add agents, specifying their start position, and store goals on the
   * opposite side of the environment (roadmap vertices). */
  for (std::size_t i = 0U; i < 5U; ++i) {
    for (std::size_t j = 0U; j < 5U; ++j) {
      simulator->addAgent(RVO::Vector2(55.0F + static_cast<float>(i) * 10.0F,
                                       55.0F + static_cast<float>(j) * 10.0F));
      goals.push_back(0);

      simulator->addAgent(RVO::Vector2(-55.0F - static_cast<float>(i) * 10.0F,
                                       55.0F + static_cast<float>(j) * 10.0F));
      goals.push_back(1);

      simulator->addAgent(RVO::Vector2(55.0F + static_cast<float>(i) * 10.0F,
                                       -55.0F - static_cast<float>(j) * 10.0F));
      goals.push_back(2);

      simulator->addAgent(RVO::Vector2(-55.0F - static_cast<float>(i) * 10.0F,
                                       -55.0F - static_cast<float>(j) * 10.0F));
      goals.push_back(3);
    }
  }
}

#if RVO_OUTPUT_TIME_AND_POSITIONS
void updateVisualization(RVO::RVOSimulator *simulator) {
  /* Output the current global time. */
  std::cout << simulator->getGlobalTime();
  RVO::Vector2 origin = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

  /* Output the current position of all the agents. */
  for (std::size_t i = 0U; i < simulator->getNumAgents(); ++i) {
    RVO::Vector2 agentPosition = simulator->getAgentPosition(i);
    float agentRadius = simulator->getAgentRadius(i);
    std::cout << " " << agentPosition;
    DrawCircle(agentPosition.x() + origin.x(), agentPosition.y() + origin.y(), agentRadius, RED);
  }

  /* Draw all the obstacles as polygons. */
  std::vector<bool> drawnVertices(simulator->getNumObstacleVertices(), false);
  for (std::size_t i = 0U; i < simulator->getNumObstacleVertices(); ++i) {
    if (!drawnVertices[i]) {
      std::size_t startVertex = i;
      std::size_t currentVertex = i;
      do {
        RVO::Vector2 start = simulator->getObstacleVertex(currentVertex);
        std::size_t nextVertex = simulator->getNextObstacleVertexNo(currentVertex);
        RVO::Vector2 end = simulator->getObstacleVertex(nextVertex);
        DrawLine(start.x() + origin.x(), start.y() + origin.y(),
                 end.x() + origin.x(), end.y() + origin.y(), BLUE);
        drawnVertices[currentVertex] = true;
        currentVertex = nextVertex;
      } while (currentVertex != startVertex);
    }
  }

  std::cout << std::endl;
}
#endif /* RVO_OUTPUT_TIME_AND_POSITIONS */

void buildRoadmap(
    RVO::RVOSimulator *simulator,
    std::vector<RoadmapVertex> &roadmap) { /* NOLINT(runtime/references) */
  /* Connect the roadmap vertices by edges if mutually visible. */
#ifdef _OPENMP
#pragma omp parallel for
#endif /* _OPENMP */
  for (int i = 0; i < static_cast<int>(roadmap.size()); ++i) {
    for (int j = 0; j < static_cast<int>(roadmap.size()); ++j) {
      if (simulator->queryVisibility(roadmap[i].position, roadmap[j].position,
                                     simulator->getAgentRadius(0U))) {
        roadmap[i].neighbors.push_back(j);
      }
    }

    /* Initialize the distance to each of the four goal vertices at infinity. */
    roadmap[i].distToGoal.resize(4U, std::numeric_limits<float>::infinity());
  }

  /* Compute the distance to each of the four goals (the first four vertices)
   * for all vertices using Dijkstra's algorithm. */
#ifdef _OPENMP
#pragma omp parallel for
#endif /* _OPENMP */
  for (int i = 0; i < 4; ++i) {
    std::multimap<float, int> Q;
    std::vector<std::multimap<float, int>::iterator> posInQ(roadmap.size(),
                                                            Q.end());

    roadmap[i].distToGoal[i] = 0.0F;
    posInQ[i] = Q.insert(std::make_pair(0.0F, i));

    while (!Q.empty()) {
      const int u = Q.begin()->second;
      Q.erase(Q.begin());
      posInQ[u] = Q.end();

      for (int j = 0; j < static_cast<int>(roadmap[u].neighbors.size()); ++j) {
        const int v = roadmap[u].neighbors[j];
        const float distUV =
            RVO::abs(roadmap[v].position - roadmap[u].position);

        if (roadmap[v].distToGoal[i] > roadmap[u].distToGoal[i] + distUV) {
          roadmap[v].distToGoal[i] = roadmap[u].distToGoal[i] + distUV;

          if (posInQ[v] == Q.end()) {
            posInQ[v] = Q.insert(std::make_pair(roadmap[v].distToGoal[i], v));
          } else {
            Q.erase(posInQ[v]);
            posInQ[v] = Q.insert(std::make_pair(roadmap[v].distToGoal[i], v));
          }
        }
      }
    }
  }
}

void setPreferredVelocities(RVO::RVOSimulator *simulator,
                            const std::vector<RoadmapVertex> &roadmap,
                            const std::vector<int> &goals) {
  /* Set the preferred velocity to be a vector of unit magnitude (speed) in the
   * direction of the visible roadmap vertex that is on the shortest path to the
   * goal. */
#ifdef _OPENMP
#pragma omp parallel for
#endif /* _OPENMP */
  for (int i = 0; i < static_cast<int>(simulator->getNumAgents()); ++i) {
    float minDist = std::numeric_limits<float>::infinity();
    int minVertex = -1;

    for (int j = 0; j < static_cast<int>(roadmap.size()); ++j) {
      if (RVO::abs(roadmap[j].position - simulator->getAgentPosition(i)) +
                  roadmap[j].distToGoal[goals[i]] <
              minDist &&
          simulator->queryVisibility(simulator->getAgentPosition(i),
                                     roadmap[j].position,
                                     simulator->getAgentRadius(i))) {
        minDist =
            RVO::abs(roadmap[j].position - simulator->getAgentPosition(i)) +
            roadmap[j].distToGoal[goals[i]];
        minVertex = j;
      }
    }

    if (minVertex == -1) {
      /* No roadmap vertex is visible; should not happen. */
      simulator->setAgentPrefVelocity(i, RVO::Vector2(0.0F, 0.0F));
    } else {
      if (RVO::absSq(roadmap[minVertex].position -
                     simulator->getAgentPosition(i)) == 0.0F) {
        if (minVertex == goals[i]) {
          simulator->setAgentPrefVelocity(i, RVO::Vector2());
        } else {
          simulator->setAgentPrefVelocity(
              i, RVO::normalize(roadmap[goals[i]].position -
                                simulator->getAgentPosition(i)));
        }
      } else {
        simulator->setAgentPrefVelocity(
            i, RVO::normalize(roadmap[minVertex].position -
                              simulator->getAgentPosition(i)));
      }
    }

    /* Perturb a little to avoid deadlocks due to perfect symmetry. */
    float angle = static_cast<float>(std::rand()) * RVO_TWO_PI /
                  static_cast<float>(RAND_MAX);
    float dist = static_cast<float>(std::rand()) * 0.0001F /
                 static_cast<float>(RAND_MAX);

    simulator->setAgentPrefVelocity(
        i, simulator->getAgentPrefVelocity(i) +
               dist * RVO::Vector2(std::cos(angle), std::sin(angle)));
  }
}

bool reachedGoal(RVO::RVOSimulator *simulator,
                 const std::vector<RoadmapVertex> &roadmap,
                 const std::vector<int> &goals) {
  /* Check if all agents have reached their goals. */
  for (std::size_t i = 0U; i < simulator->getNumAgents(); ++i) {
    if (RVO::absSq(simulator->getAgentPosition(i) -
                   roadmap[goals[i]].position) > 400.0F) {
      return false;
    }
  }

  return true;
}
} /* namespace */

int main() {
  /* Store the roadmap. */
  std::vector<RoadmapVertex> roadmap;

  /* Store the goals of the agents. */
  std::vector<int> goals;

  /* Create a new simulator instance. */
  RVO::RVOSimulator *simulator = new RVO::RVOSimulator();

  /* Set up the scenario. */
  setupScenario(simulator, roadmap, goals);

  /* Build the roadmap. */
  buildRoadmap(simulator, roadmap);

  /* Perform and manipulate the simulation. */
  InitWindow(800, 600, "Roadmap ORCA Visualization");
  do {
    BeginDrawing();
    ClearBackground(BLACK);
#if RVO_OUTPUT_TIME_AND_POSITIONS
    updateVisualization(simulator);
#endif /* RVO_OUTPUT_TIME_AND_POSITIONS */
    setPreferredVelocities(simulator, roadmap, goals);
    simulator->doStep();
    EndDrawing();
  } while (!reachedGoal(simulator, roadmap, goals));

  CloseWindow();
  delete simulator;

  return 0;
}
