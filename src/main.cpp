#include "graph_logic/graph.hpp"
int main() 
{
    int no_of_equation=1; 
    cout <<"Enter an number equation"<<endl;
    cin>>no_of_equation; 
    vector<vector<string>> example;
    vector <int> bracket_counter;
    int x;
    vector<string>color_list;
    for(int i=0;i<no_of_equation;i++){
        int bracket=0;
        string equation,colors;
        cout <<"Enter an equation "<<(i+1)<<endl;
        cin>>ws;
        getline(cin, equation);
        cout <<"Enter your color for equation "<<(i+1)<<endl;
        cin>>colors;
        color_list.push_back(colors);
        vector<string> subexample;
        subexample=parser(equation,bracket);
        example.push_back(subexample);
        bracket_counter.push_back(bracket);
    }
    
    double start=0;
    double stop=1;
    double step=0.001;
    cout<<"enter your start: "<<endl;
    cin>>start;
    cout<<"enter your stop:"<<endl;
    cin>>stop;
    Node* eq=NULL;
    vector<vector<double>> yvalues;
    vector<vector<double>> xvalues;
    int i=0;
    for(int no=0;no<no_of_equation;no++){
        eq=build_ast(example[no],start,i);
        vector<double> subyvalues;
        vector<double> subxvalues;
        for(double k=start;k<=stop;k+=step){
            for(int j=0;j<=bracket_counter[no]+1;j++){
                try_solve(eq,k);
        }
        //if(!(std::isfinite(eq->result))){
        //  if(no<no_of_equation-1)no++;
        // continue;
        //}
            subyvalues.push_back(eq->result);
            subxvalues.push_back(k);
        }
        yvalues.push_back(subyvalues);
        xvalues.push_back(subxvalues);
        free_ast(eq);
    }
    LineBatchRenderer batchRenderer;
    ColorManager colorManager;
    sf::Clock clock;

    Camera camera;

    // Zoom parameters (cursor-relative zoom)
    const float zoomFactor = 1.2f;
    const float minScale = 0.1f;
    const float maxScale = 500.0f;

    // Panning state (middle mouse drag)
    bool isPanning = false;
    sf::Vector2f panStartMouse;
    sf::Vector2f panStartOffset;

    // Cache geometry so we don't rebuild all vertices every frame
    bool geometryDirty = true;

    sf::RenderWindow window( sf::VideoMode( { SCREEN_WIDTH, SCREEN_HEIGHT } ), "SFMLstuff!" );
    window.setFramerateLimit(60);
	while ( window.isOpen() ){
        float deltaTime = clock.restart().asSeconds();
        
            while ( const std::optional event = window.pollEvent() )
            {
            
                if ( event->is<sf::Event::Closed>())window.close();

                if (const auto* wheelEv = event->getIf<sf::Event::MouseWheelScrolled>()) {
                    if (wheelEv->wheel == sf::Mouse::Wheel::Vertical && wheelEv->delta != 0.0f) {
                        const sf::Vector2f mousePos(static_cast<float>(wheelEv->position.x),
                                                    static_cast<float>(wheelEv->position.y));

                        // Keep the graph coordinate under the cursor fixed while zooming
                        const sf::Vector2f graphPosBefore = camera.screenToGraph(mousePos.x, mousePos.y);

                        const float zoom = (wheelEv->delta > 0.0f) ? zoomFactor : (1.0f / zoomFactor);
                        camera.scaleX = std::clamp(camera.scaleX * zoom, minScale, maxScale);
                        camera.scaleY = std::clamp(camera.scaleY * zoom, minScale, maxScale);

                        const sf::Vector2f graphPosAfter = camera.screenToGraph(mousePos.x, mousePos.y);
                        camera.offsetX += graphPosAfter.x - graphPosBefore.x;
                        camera.offsetY += graphPosAfter.y - graphPosBefore.y;

                        geometryDirty = true;
                    }
                }

                if (const auto* pressEv = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (pressEv->button == sf::Mouse::Button::Middle) {
                        isPanning = true;
                        panStartMouse = sf::Vector2f(static_cast<float>(pressEv->position.x),
                                                     static_cast<float>(pressEv->position.y));
                        panStartOffset = sf::Vector2f(camera.offsetX, camera.offsetY);
                    }
                }

                if (const auto* releaseEv = event->getIf<sf::Event::MouseButtonReleased>()) {
                    if (releaseEv->button == sf::Mouse::Button::Middle) {
                        isPanning = false;
                    }
                }

                if (const auto* moveEv = event->getIf<sf::Event::MouseMoved>()) {
                    if (isPanning) {
                        const sf::Vector2f currentMouse(static_cast<float>(moveEv->position.x),
                                                        static_cast<float>(moveEv->position.y));
                        const sf::Vector2f delta = currentMouse - panStartMouse;

                        // Convert pixel delta to graph-coordinate delta
                        camera.offsetX = panStartOffset.x - delta.x / camera.scaleX;
                        camera.offsetY = panStartOffset.y + delta.y / camera.scaleY;

                        geometryDirty = true;
                    }
                }

                if (const auto* keyEv = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyEv->code == sf::Keyboard::Key::Escape) {
                        window.close();
                    }
                    if (keyEv->code == sf::Keyboard::Key::Home) {
                        camera = Camera{};
                        geometryDirty = true;
                    }
                }
            }

            if (geometryDirty) {
                batchRenderer.clear();
                drawAxes(batchRenderer, camera);
                drawGraphLines(xvalues, yvalues, color_list, batchRenderer, colorManager, camera);
                geometryDirty = false;
            }
        
            // Rendering
            window.clear(sf::Color(20, 20, 30)); // Clear with background color
        
            // Draw all batched lines at once (much faster than individual draw calls)
            batchRenderer.render(window);

            // Optional: draw a simple crosshair at the cursor
            const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (mousePos.x >= 0 && mousePos.x < SCREEN_WIDTH &&
                mousePos.y >= 0 && mousePos.y < SCREEN_HEIGHT) {
                const float mx = static_cast<float>(mousePos.x);
                const float my = static_cast<float>(mousePos.y);

                const sf::Vertex crosshair[] = {
                    {{mx - 10.0f, my}, sf::Color::Green},
                    {{mx + 10.0f, my}, sf::Color::Green},
                    {{mx, my - 10.0f}, sf::Color::Green},
                    {{mx, my + 10.0f}, sf::Color::Green},
                };
                window.draw(crosshair, 4, sf::PrimitiveType::Lines);
            }
        
            window.display();
    }
        
}
    
