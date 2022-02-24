#pragma once
namespace Quartz::Editor
{
class EditorWindow
{
public:
	virtual void initialize() = 0;
	virtual void update() = 0;
	virtual void render() = 0;

	bool getVisible() const { return m_visible; }
	void setVisible(bool visible) { m_visible = visible; }
	const std::string& getName() const { return m_name; }
	void setName(const std::string& name) { m_name = name; }

protected:
	bool m_visible = false;
	std::string m_name = "NoName";
	float m_width = 100;
	float m_height = 100;
	bool m_resized = false;
};
}