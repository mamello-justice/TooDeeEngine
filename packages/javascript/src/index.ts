export namespace TooDeeCore {
  export interface Vec2 {
    x: number;
    y: number;
  }
}

export namespace SF {
  export class RenderTexture {
    getSize(): TooDeeCore.Vec2 {
      throw new Error('Not implemented');
    }
  }
}

export namespace TooDeeEngine {
  // Components
  export interface CAnimation { }

  export interface CBoundingBox {
    size: TooDeeCore.Vec2;
    halfSize: TooDeeCore.Vec2;
  }

  export interface CBoundingCircle {
    radius: number;
  }

  export interface CSprite { }

  export interface CText { }

  export interface CRectangle { }

  export interface CScore { }

  export interface CState { }

  export interface CTransform {
    pos: TooDeeCore.Vec2;
    prevPos: TooDeeCore.Vec2;
    scale: TooDeeCore.Vec2;
    velocity: TooDeeCore.Vec2;
    angle: number;
  }

  export interface CQJSScript { }

  export enum ComponentEnum {
    Animation,
    BoundingBox,
    BoundingCircle,
    Sprite,
    Text,
    Rectangle,
    Score,
    State,
    Transform,
    JavascriptScript,
  }

  export type ComponentType<C extends ComponentEnum> =
    C extends ComponentEnum.Animation
    ? CAnimation
    : C extends ComponentEnum.BoundingBox
    ? CBoundingBox
    : C extends ComponentEnum.BoundingCircle
    ? CBoundingCircle
    : C extends ComponentEnum.Sprite
    ? CSprite
    : C extends ComponentEnum.Text
    ? CText
    : C extends ComponentEnum.Rectangle
    ? CRectangle
    : C extends ComponentEnum.Score
    ? CScore
    : C extends ComponentEnum.State
    ? CState
    : C extends ComponentEnum.Transform
    ? CTransform
    : C extends ComponentEnum.JavascriptScript
    ? CQJSScript
    : never;

  // Entity
  export interface Entity {
    hasComponent(component: ComponentEnum): boolean;
    getComponent<C extends ComponentEnum>(component: C): ComponentType<C>;
  }

  // Script
  export interface Script {
    onUpdate(entity: Entity): void;
  }

  // Engine
  export const getRenderTarget: () => SF.RenderTexture = () => {
    throw new Error('Not implemented');
  };
}
