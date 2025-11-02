#pragma once

#include <string>

class Action {
	std::string m_name = "NONE";
	std::string m_type = "NONE";

public:
	/**
	 * @brief Default constructor
	 */
	Action() = default;

	/**
	 * @brief Parameterized constructor
	 *
	 * @param name The name of the action
	 * @param type The type of the action
	 */
	Action(const std::string& name, const std::string& type);

	/**
	 * @brief Get the name of the action
	 *
	 * @return The name of the action
	 */
	const std::string& name() const;

	/**
	 * @brief Get the type of the action
	 *
	 * @return The type of the action
	 */
	const std::string& type() const;
};
