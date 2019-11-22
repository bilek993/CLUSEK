#include "PhysicsStatisticsWindow.h"
#include <imgui.h>

void PhysicsStatisticsWindow::Draw()
{
	ImGui::Begin("Physics statistics", &IsEnabled);

	const auto physicsSystem = FindPhysicsSystem();

	if (!physicsSystem)
		DrawNotImplementedInfo();
	else
		DrawStats(physicsSystem);

	ImGui::End();
}

PhysicsSystem* PhysicsStatisticsWindow::FindPhysicsSystem() const
{
	for (auto &system : *Systems)
	{
		const auto physicsSystem = dynamic_cast<PhysicsSystem*>(system.System.get());
		if (physicsSystem)
			return physicsSystem;
	}

	return nullptr;
}

void PhysicsStatisticsWindow::DrawNotImplementedInfo() const
{
	ImGui::Text("Physics System not found!");
	ImGui::Text("Add physics system to your project.");
}

void PhysicsStatisticsWindow::DrawStats(const PhysicsSystem* physicsSystem) const
{
	const auto statistics = physicsSystem->GetStatistics();

	ImGui::Text("Number of active constraints: %d", statistics.nbActiveConstraints);
	ImGui::Text("Number of active dynamic bodies: %d", statistics.nbActiveDynamicBodies);
	ImGui::Text("Number of active kinematic bodies: %d", statistics.nbActiveKinematicBodies);
	ImGui::Text("Number of static bodies: %d", statistics.nbStaticBodies);
	ImGui::Text("Number of dynamic bodies: %d", statistics.nbDynamicBodies);
	ImGui::Text("Number of kinematic bodies: %d", statistics.nbKinematicBodies);
	ImGui::Text("Number of aggregates: %d", statistics.nbAggregates);
	ImGui::Text("Number of articulations: %d", statistics.nbArticulations);

	ImGui::Separator();

	ImGui::Text("Number of axis solver constraints: %d", statistics.nbAxisSolverConstraints);
	ImGui::Text("Compressed contact size: %d", statistics.compressedContactSize);
	ImGui::Text("required contact constraint memory: %d", statistics.requiredContactConstraintMemory);
	ImGui::Text("Peak constraint memory: %d", statistics.peakConstraintMemory);

	ImGui::Separator();

	ImGui::Text("Number of broadphase adds: %d", statistics.getNbBroadPhaseAdds());
	ImGui::Text("Number of broadphase removes:: %d", statistics.getNbBroadPhaseRemoves());

	ImGui::Separator();

	ImGui::Text("Number of discrete contact pairs: %d", statistics.nbDiscreteContactPairsTotal);
	ImGui::Text("Number of discrete contact pairs with cache hits %d", statistics.nbDiscreteContactPairsWithCacheHits);
	ImGui::Text("Number of discrete contact pairs with contacts: %d", statistics.nbDiscreteContactPairsWithContacts);

	ImGui::Separator();

	ImGui::Text("Number of new pairs: %d", statistics.nbNewPairs);
	ImGui::Text("Number of lost pairs: %d", statistics.nbLostPairs);
	ImGui::Text("Number of new touches: %d", statistics.nbNewTouches);
	ImGui::Text("Number of lost touches: %d", statistics.nbLostTouches);
	ImGui::Text("Number of partitions: %d", statistics.nbPartitions);
}
