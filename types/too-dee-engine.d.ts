declare global {
    export declare namespace TooDeeCore {
        interface Vec2 {
            x: number;
            y: number;
        }
    }
    export declare namespace SF {
        class RenderTexture {
            getSize(): TooDeeCore.Vec2;
        }
    }
    export declare namespace TooDeeEngine {
        interface CAnimation {
        }
        interface CBoundingBox {
            size: TooDeeCore.Vec2;
            halfSize: TooDeeCore.Vec2;
        }
        interface CBoundingCircle {
            radius: number;
        }
        interface CSprite {
        }
        interface CText {
        }
        interface CRectangle {
        }
        interface CScore {
        }
        interface CState {
        }
        interface CTransform {
            pos: TooDeeCore.Vec2;
            prevPos: TooDeeCore.Vec2;
            scale: TooDeeCore.Vec2;
            velocity: TooDeeCore.Vec2;
            angle: number;
        }
        interface CQJSScript {
        }
        enum ComponentEnum {
            Animation = 0,
            BoundingBox = 1,
            BoundingCircle = 2,
            Sprite = 3,
            Text = 4,
            Rectangle = 5,
            Score = 6,
            State = 7,
            Transform = 8,
            JavascriptScript = 9
        }
        type ComponentType<C extends ComponentEnum> = C extends ComponentEnum.Animation ? CAnimation : C extends ComponentEnum.BoundingBox ? CBoundingBox : C extends ComponentEnum.BoundingCircle ? CBoundingCircle : C extends ComponentEnum.Sprite ? CSprite : C extends ComponentEnum.Text ? CText : C extends ComponentEnum.Rectangle ? CRectangle : C extends ComponentEnum.Score ? CScore : C extends ComponentEnum.State ? CState : C extends ComponentEnum.Transform ? CTransform : C extends ComponentEnum.JavascriptScript ? CQJSScript : never;
        interface Components {
            transform?: CTransform;
            boundingBox?: CBoundingBox;
            boundingCircle?: CBoundingCircle;
        }
        interface Entity {
            components: Components;
        }
        interface RenderTarget {
            size: TooDeeCore.Vec2;
        }
        const renderTarget: RenderTarget;
    }
}

export { };